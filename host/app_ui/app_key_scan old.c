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
 * @file      key_scan.c
 * @brief     Module related to key scan
 * @author    Phil
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "reg_def.h"
#include "timer.h"
#include "gpio.h"
#include "gpio_api.h"
#include "key_scan.h"
#include "os_msg.h"
#include "app_msg.h"
#include "app_dev_storage.h"
#include "app_dev_usb.h"
#include "app_dev_disp.h"
#include "sp1k_util_api.h"
#include "sp1k_gpio_api.h"
#include "sp1k_hal_api.h"
#include "App_ui_osd.h"
#include "app_video_view.h"
#include "gui_core.h"
#include "gui_evt.h"
#include "gui_tp.h"
#include "app_tp_scan.h"
#include "sp1k_video_api.h"
#include "video_pb.h"
#include "App_ui_para.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define USE_PKG_DEDICATE_PIN  1  /* 0: old code, 1: new code. */
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 #define KEY_VALUE_AVEAGE 0
#if 0
#define DBG_KEYSCAN
#endif
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
   UINT8 keyFlag = 0;
   UINT8 prevKeyVal = 0;
   UINT8 flag1627 = 0;
#if KEY_VALUE_AVEAGE
   UINT8 keepChannelFlag=0;//  1: keep channel 0 :enable change channel
#endif
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
extern xdata UINT8 G_DgpioValue;
extern UINT8 affarflag;
#if (_HW_SOLUTION_ == _HW_CUSTOM_MODEL_A_||_HW_SOLUTION_ == _HW_CUSTOM_MODEL_B_)
static UINT16 code keyAdcAValDevTbl[SP1K_ADC_BTN_A_NUM] = {
	SP1K_ADC_BTN_DEV_A_S1,
	SP1K_ADC_BTN_DEV_A_S2,
	SP1K_ADC_BTN_DEV_A_S3,
	SP1K_ADC_BTN_DEV_A_S4,
};
#else
static UINT16 code keyAdcAValDevTbl[SP1K_ADC_BTN_A_NUM] = {
	SP1K_ADC_BTN_DEV_A_S1,
	SP1K_ADC_BTN_DEV_A_S2,
	SP1K_ADC_BTN_DEV_A_S3,
	SP1K_ADC_BTN_DEV_A_S4,
};
#endif





#if (_HW_SOLUTION_ == _HW_EVB_ || _HW_SOLUTION_ == _HW_TK_MODEL_B_ )
static UINT16 code keyAdcBValDevTbl[SP1K_ADC_BTN_B_NUM] = {
	SP1K_ADC_BTN_DEV_B_S1,
	SP1K_ADC_BTN_DEV_B_S2,
	SP1K_ADC_BTN_DEV_B_S3,
	SP1K_ADC_BTN_DEV_B_S4,
	SP1K_ADC_BTN_DEV_B_S5,
	SP1K_ADC_BTN_DEV_B_S6,
	//SP1K_ADC_BTN_DEV_B_S7,
};

#elif (_HW_SOLUTION_ == _HW_CUSTOM_MODEL_A_)
static UINT16 code keyAdcBValDevTbl[SP1K_ADC_BTN_B_NUM] = {

	SP1K_ADC_BTN_DEV_B_S1,
	SP1K_ADC_BTN_DEV_B_S2,
};
#elif (_HW_SOLUTION_ == _HW_CUSTOM_MODEL_B_)
static UINT16 code keyAdcBValDevTbl[SP1K_ADC_BTN_B_NUM] = {

	SP1K_ADC_BTN_DEV_B_S1,
	SP1K_ADC_BTN_DEV_B_S2,
	SP1K_ADC_BTN_DEV_B_S3,
};

#elif (_HW_SOLUTION_ == _HW_RDK_A_)
static UINT16 code keyAdcBValDevTbl[SP1K_ADC_BTN_B_NUM] = {

	SP1K_ADC_BTN_DEV_B_S1,
	SP1K_ADC_BTN_DEV_B_S2,
	SP1K_ADC_BTN_DEV_B_S3,
	SP1K_ADC_BTN_DEV_B_S4,
};
#elif (_HW_SOLUTION_ == _HW_TK_MODEL_A_)
static UINT16 code keyAdcBValDevTbl[SP1K_ADC_BTN_B_NUM] = {
	SP1K_ADC_BTN_DEV_B_S1,
	SP1K_ADC_BTN_DEV_B_S2,
	SP1K_ADC_BTN_DEV_B_S3,
};
#endif


#if(_HW_SOLUTION_ == _HW_CUSTOM_MODEL_A_ || _HW_SOLUTION_ == _HW_CUSTOM_MODEL_B_)
static UINT16 code keyAdcAVal[SP1K_ADC_BTN_A_NUM] = {
	SP1K_ADC_BTN_A_S1,
	SP1K_ADC_BTN_A_S2,
	SP1K_ADC_BTN_A_S3,
	SP1K_ADC_BTN_A_S4,
};
#else
static UINT16 code keyAdcAVal[SP1K_ADC_BTN_A_NUM] = {
	SP1K_ADC_BTN_A_S1,
	SP1K_ADC_BTN_A_S2,
	SP1K_ADC_BTN_A_S3,
	SP1K_ADC_BTN_A_S4,
};
#endif


#if (_HW_SOLUTION_ == _HW_EVB_ || _HW_SOLUTION_ == _HW_TK_MODEL_B_ )
static UINT16 code keyAdcBVal[SP1K_ADC_BTN_B_NUM] = {
	SP1K_ADC_BTN_B_S1,
	SP1K_ADC_BTN_B_S2,
	SP1K_ADC_BTN_B_S3,
	SP1K_ADC_BTN_B_S4,
	SP1K_ADC_BTN_B_S5,
	SP1K_ADC_BTN_B_S6,
	//SP1K_ADC_BTN_B_S7,
};
#elif (_HW_SOLUTION_ == _HW_RDK_A_)

static UINT16 code keyAdcBVal[SP1K_ADC_BTN_B_NUM] = {

	SP1K_ADC_BTN_B_S1,
	SP1K_ADC_BTN_B_S2,
	SP1K_ADC_BTN_B_S3,
	SP1K_ADC_BTN_B_S4,

};
#elif (_HW_SOLUTION_ == _HW_CUSTOM_MODEL_A_)

static UINT16 code keyAdcBVal[SP1K_ADC_BTN_B_NUM] = {

	SP1K_ADC_BTN_B_S1,
	SP1K_ADC_BTN_B_S2,
};
#elif (_HW_SOLUTION_ == _HW_CUSTOM_MODEL_B_)

static UINT16 code keyAdcBVal[SP1K_ADC_BTN_B_NUM] = {

	SP1K_ADC_BTN_B_S1,
	SP1K_ADC_BTN_B_S2,
	SP1K_ADC_BTN_B_S3,
};
#elif (_HW_SOLUTION_ == _HW_TK_MODEL_A_)
static UINT16 code keyAdcBVal[SP1K_ADC_BTN_B_NUM] = {
	SP1K_ADC_BTN_B_S1,
	SP1K_ADC_BTN_B_S2,
	SP1K_ADC_BTN_B_S3,
};
#endif

/*Key message definition*/

static keyMsg_t code keyMsg[SP1K_BTN_NUM] = {

#if (_HW_SOLUTION_ == _HW_EVB_)

	/*ADC A button msg*/
	{SP1K_MSG_KEY_PRESS_DOWN, 			SP1K_MSG_KEY_REPEATE_DOWN,			SP1K_MSG_KEY_RELEASE_DOWN},//left
	{SP1K_MSG_KEY_PRESS_PB, 			SP1K_MSG_KEY_REPEATE_PB,			SP1K_MSG_KEY_RELEASE_PB},//del
	{SP1K_MSG_KEY_PRESS_OK, 			SP1K_MSG_NULL,						SP1K_MSG_KEY_RELEASE_OK},//ok
	{SP1K_MSG_KEY_PRESS_POWER,			SP1K_MSG_KEY_REPEATE_POWER, 		SP1K_MSG_KEY_RELEASE_POWER},//power

	/*ADC B button msg*/
	{SP1K_MSG_KEY_PRESS_AF, 			SP1K_MSG_NULL, 			SP1K_MSG_NULL},//af
	//{SP1K_MSG_KEY_PRESS_MENU,			SP1K_MSG_KEY_REPEATE_MENU,			SP1K_MSG_KEY_RELEASE_MENU},
	{SP1K_MSG_KEY_PRESS_BURST, 			SP1K_MSG_KEY_REPEATE_BURST,			SP1K_MSG_KEY_RELEASE_BURST},//right
	{SP1K_MSG_KEY_PRESS_UP, 			SP1K_MSG_KEY_REPEATE_UP,			SP1K_MSG_KEY_RELEASE_UP},//up
	{SP1K_MSG_KEY_PRESS_ZOOM_IN,		SP1K_MSG_KEY_REPEATE_ZOOM_IN, 		SP1K_MSG_KEY_RELEASE_ZOOM_IN},//t
	{SP1K_MSG_KEY_PRESS_ZOOM_OUT,		SP1K_MSG_KEY_REPEATE_ZOOM_OUT, 		SP1K_MSG_KEY_RELEASE_ZOOM_OUT},//w
	{SP1K_MSG_KEY_PRESS_SNAP, 			SP1K_MSG_KEY_REPEATE_SNAP, 			SP1K_MSG_KEY_RELEASE_SNAP},//shutter
	
	/*GPIO button msg*/
	{SP1K_MSG_KEY_PRESS_MENU,			SP1K_MSG_KEY_REPEATE_MENU,			SP1K_MSG_KEY_RELEASE_MENU},
	{SP1K_MSG_KEY_PRESS_MODE,			SP1K_MSG_NULL,						SP1K_MSG_KEY_RELEASE_MODE},

#elif (_HW_SOLUTION_ == _HW_TK_MODEL_A_)

	/*ADC A button msg*/
	{SP1K_MSG_KEY_PRESS_RIGHT,		SP1K_MSG_KEY_REPEATE_RIGHT, 	SP1K_MSG_KEY_RELEASE_RIGHT},
	{SP1K_MSG_KEY_PRESS_LEFT, 		SP1K_MSG_KEY_REPEATE_LEFT, 		SP1K_MSG_KEY_RELEASE_LEFT},
	{SP1K_MSG_KEY_PRESS_DOWN, 	SP1K_MSG_KEY_REPEATE_DOWN, 	SP1K_MSG_KEY_RELEASE_DOWN},
	{SP1K_MSG_KEY_PRESS_UP, 		SP1K_MSG_KEY_REPEATE_UP, 		SP1K_MSG_KEY_RELEASE_UP},
	{SP1K_MSG_KEY_PRESS_TELE, 		SP1K_MSG_KEY_REPEATE_TELE, 		SP1K_MSG_KEY_RELEASE_TELE},
	{SP1K_MSG_KEY_PRESS_WIDE, 		SP1K_MSG_KEY_REPEATE_WIDE, 	SP1K_MSG_KEY_RELEASE_WIDE},

	/*ADC B button msg*/
	{SP1K_MSG_KEY_PRESS_OK, 		SP1K_MSG_NULL, 					SP1K_MSG_KEY_RELEASE_OK},
	{SP1K_MSG_KEY_PRESS_PB, 		SP1K_MSG_NULL, 					SP1K_MSG_KEY_RELEASE_PB},
	{SP1K_MSG_KEY_PRESS_MENU, 		SP1K_MSG_NULL, 					SP1K_MSG_KEY_RELEASE_MENU},

	/*GPIO button msg*/
	{SP1K_MSG_KEY_PRESS_POWER, 	SP1K_MSG_NULL, 					SP1K_MSG_KEY_RELEASE_POWER},
	{SP1K_MSG_KEY_PRESS_S2, 		SP1K_MSG_NULL, 					SP1K_MSG_KEY_RELEASE_S2},

#elif (_HW_SOLUTION_ == _HW_TK_MODEL_B_)
	/*ADC A button msg*/
	{SP1K_MSG_KEY_PRESS_RIGHT,		SP1K_MSG_KEY_REPEATE_RIGHT, 	SP1K_MSG_KEY_RELEASE_RIGHT},
	{SP1K_MSG_KEY_PRESS_LEFT, 		SP1K_MSG_KEY_REPEATE_LEFT, 		SP1K_MSG_KEY_RELEASE_LEFT},
	{SP1K_MSG_KEY_PRESS_DOWN, 	SP1K_MSG_KEY_REPEATE_DOWN, 	SP1K_MSG_KEY_RELEASE_DOWN},
	{SP1K_MSG_KEY_PRESS_UP, 		SP1K_MSG_KEY_REPEATE_UP, 		SP1K_MSG_KEY_RELEASE_UP},
	{SP1K_MSG_KEY_PRESS_TELE, 		SP1K_MSG_KEY_REPEATE_TELE, 		SP1K_MSG_KEY_RELEASE_TELE},
	{SP1K_MSG_KEY_PRESS_WIDE, 		SP1K_MSG_KEY_REPEATE_WIDE, 	SP1K_MSG_KEY_RELEASE_WIDE},

	/*ADC B button msg*/
	{SP1K_MSG_KEY_PRESS_POWER, 	SP1K_MSG_NULL, 					SP1K_MSG_KEY_RELEASE_POWER},
	{SP1K_MSG_KEY_PRESS_OK, 		SP1K_MSG_NULL, 					SP1K_MSG_KEY_RELEASE_OK},
	{SP1K_MSG_KEY_PRESS_PB, 		SP1K_MSG_NULL, 					SP1K_MSG_KEY_RELEASE_PB},
	{SP1K_MSG_KEY_PRESS_MENU, 		SP1K_MSG_NULL, 					SP1K_MSG_KEY_RELEASE_MENU},

	/*GPIO button msg*/
	{SP1K_MSG_NULL, 				SP1K_MSG_NULL, 					SP1K_MSG_NULL},
	{SP1K_MSG_KEY_PRESS_S2, 		SP1K_MSG_NULL, 					SP1K_MSG_KEY_RELEASE_S2},

#elif (_HW_SOLUTION_ == _HW_RDK_A_)

	/*ADC A button msg*/
	{SP1K_MSG_KEY_PRESS_RIGHT,		SP1K_MSG_KEY_REPEATE_RIGHT, 	SP1K_MSG_KEY_RELEASE_RIGHT},
	{SP1K_MSG_KEY_PRESS_LEFT,		SP1K_MSG_KEY_REPEATE_LEFT,		SP1K_MSG_KEY_RELEASE_LEFT},
	{SP1K_MSG_KEY_PRESS_DOWN,	SP1K_MSG_KEY_REPEATE_DOWN,	SP1K_MSG_KEY_RELEASE_DOWN},
	{SP1K_MSG_KEY_PRESS_UP, 		SP1K_MSG_KEY_REPEATE_UP,		SP1K_MSG_KEY_RELEASE_UP},
	{SP1K_MSG_KEY_PRESS_TELE,		SP1K_MSG_KEY_REPEATE_TELE,		SP1K_MSG_KEY_RELEASE_TELE},
	{SP1K_MSG_KEY_PRESS_WIDE,		SP1K_MSG_KEY_REPEATE_WIDE,	SP1K_MSG_KEY_RELEASE_WIDE},

	/*ADC B button msg*/
	{SP1K_MSG_KEY_PRESS_MENU,		SP1K_MSG_NULL,					SP1K_MSG_KEY_RELEASE_MENU},
	{SP1K_MSG_KEY_PRESS_PB, 		SP1K_MSG_NULL,					SP1K_MSG_KEY_RELEASE_PB},
	//{SP1K_MSG_KEY_PRESS_F1,		SP1K_MSG_NULL,					SP1K_MSG_KEY_RELEASE_F1},
	{SP1K_MSG_KEY_PRESS_OK, 		SP1K_MSG_NULL,					SP1K_MSG_KEY_RELEASE_OK},
	{SP1K_MSG_KEY_PRESS_POWER,		SP1K_MSG_NULL,					SP1K_MSG_KEY_RELEASE_POWER},

	/*GPIO button msg*/
	{SP1K_MSG_KEY_PRESS_S2, 		SP1K_MSG_NULL, 					SP1K_MSG_KEY_RELEASE_S2},
#elif (_HW_SOLUTION_ == _HW_CUSTOM_MODEL_A_)

	/*ADC A button msg*/
	{SP1K_MSG_KEY_PRESS_RIGHT,		SP1K_MSG_KEY_REPEATE_RIGHT, 	SP1K_MSG_KEY_RELEASE_RIGHT},
	{SP1K_MSG_KEY_PRESS_LEFT,		SP1K_MSG_KEY_REPEATE_LEFT,		SP1K_MSG_KEY_RELEASE_LEFT},
	{SP1K_MSG_KEY_PRESS_DOWN,		SP1K_MSG_KEY_REPEATE_DOWN,		SP1K_MSG_KEY_RELEASE_DOWN},
	{SP1K_MSG_KEY_PRESS_UP, 		SP1K_MSG_KEY_REPEATE_UP,		SP1K_MSG_KEY_RELEASE_UP},

	/*ADC B button msg*/
	{SP1K_MSG_KEY_PRESS_MENU,		SP1K_MSG_NULL,					SP1K_MSG_KEY_RELEASE_MENU},
	{SP1K_MSG_KEY_PRESS_PB, 		SP1K_MSG_NULL,					SP1K_MSG_KEY_RELEASE_PB},
	/*GPIO button msg*/

	{SP1K_MSG_KEY_PRESS_POWER,		SP1K_MSG_NULL,					SP1K_MSG_KEY_RELEASE_POWER},
	{SP1K_MSG_KEY_PRESS_OK, 		SP1K_MSG_NULL,					SP1K_MSG_KEY_RELEASE_OK},
#elif (_HW_SOLUTION_ == _HW_CUSTOM_MODEL_B_)
	/*ADC A button msg*/
	{SP1K_MSG_KEY_PRESS_RIGHT,		SP1K_MSG_KEY_REPEATE_RIGHT, 	SP1K_MSG_KEY_RELEASE_RIGHT},
	{SP1K_MSG_KEY_PRESS_LEFT,		SP1K_MSG_KEY_REPEATE_LEFT,		SP1K_MSG_KEY_RELEASE_LEFT},
	{SP1K_MSG_KEY_PRESS_DOWN,		SP1K_MSG_KEY_REPEATE_DOWN,		SP1K_MSG_KEY_RELEASE_DOWN},
	{SP1K_MSG_KEY_PRESS_UP, 		SP1K_MSG_KEY_REPEATE_UP,		SP1K_MSG_KEY_RELEASE_UP},

	/*ADC B button msg*/
	{SP1K_MSG_KEY_PRESS_MENU,		SP1K_MSG_NULL,					SP1K_MSG_KEY_RELEASE_MENU},//menu
	{SP1K_MSG_KEY_PRESS_F1, 		SP1K_MSG_NULL, 					SP1K_MSG_KEY_RELEASE_F1},//mode
	{SP1K_MSG_KEY_PRESS_PB, 		SP1K_MSG_NULL,					SP1K_MSG_KEY_RELEASE_PB},//PB
	/*GPIO button msg*/

	{SP1K_MSG_KEY_PRESS_POWER,		SP1K_MSG_NULL,					SP1K_MSG_KEY_RELEASE_POWER},
	{SP1K_MSG_KEY_PRESS_OK, 		SP1K_MSG_NULL,					SP1K_MSG_KEY_RELEASE_OK},

#endif //hw solution
};

static void battAdcParsing(battUnitAdc_t* pAdc);
static void keyScanAdcParsing(keyUnitAdc_t* pAdc);
static UINT8 keyScanAdcValParsing(UINT16 AdcValue, keyUnitAdc_t* pAdc, UINT8* AdcIdx);
static UINT8 keyScanAdcIdleChk(UINT16 AdcValue, keyUnitAdc_t* pAdc);
static void keyScanDgpioValParsing(keyUnitAdc_t* pAdc);
static void keyScanGpioParsing(keyUnitGpio_t* pGpio);
static void keyScanGpioValParsing(UINT8 gpioVal, UINT8 gpioChn, keyUnitGpio_t* pGpio, UINT8 gpioPrsFlag);

//static powerBatLv_t xdata BatteryLevel = 0;

void keyScanTpParsing(void);

static scanTag_t xdata sTag;
static UINT8 xdata sKeyEn[SP1K_BTN_NUM];
extern UINT8 TestFlag;
#if USE_PKG_DEDICATE_PIN
static UINT8 gpioByteIdS2, gpioBitMskS2, gpiopolarityS2;

extern void pkgDedicateGpioCfg(UINT8 pinId, UINT8 *pbyteId, UINT8 *pbitMsk, UINT8*ppolarity, UINT8 dir);
#endif
extern UINT8 HAL_GlobalDGPIOModeGet(void);

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

#if SUPPORT_PANEL_PROTECT
extern xdata UINT16 G_AutoOffPanelTime;
#endif








/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

//-----------------------------------------------------------------------------
//keyScan
//-----------------------------------------------------------------------------
/**
 * @fn        void keyScan(void)
 * @brief     keyScan
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-18
 * @todo      N/A
 * @bug       N/A
*/
void keyScan(void)
{
	static UINT8 scanTag = 0;

	#if SUPPORT_PANEL_PROTECT
	static UINT8 ucPanel100MsDleay = 100;
	static UINT8 PanelTime = 100;
	#endif

	#if SUPPORT_MCU_WTD
	static UINT16 heartbeatSendDelay3Sec = 1;
	static UINT8 highLowStatus = 0;
	#endif
	static UINT16 afbackfar = 500;
	

	// 屏幕保护定时
	#if SUPPORT_PANEL_PROTECT
	if(G_AutoOffPanelTime!=0xffff && sp1kVideoPbStatusGet() == VID_PB_IDLE)
	{
		if(G_AutoOffPanelTime)
		{
			if(PanelTime)
				PanelTime--;
			else
			{
				PanelTime=100;
				G_AutoOffPanelTime --;
			}
		}
	}
	else
	{
		PanelTime=100;
	}
	#endif	

	#if (_HW_SOLUTION_ == _HW_CUSTOM_MODEL_B_)
	static UINT16 KeyFlagCount = 0;
	
	if(keyFlag == 0)
	{
		KeyFlagCount++;
		if(KeyFlagCount == 500)
		{
			keyFlag = 1;
			KeyFlagCount = 0;
			osMsgPost(SP1K_MSG_CLEAN_ZOOM_OSD);
		}
	}
	else{
		KeyFlagCount = 0;
	}
	#endif
	
	#if KEY_VALUE_AVEAGE
		keepChannelFlag=0;
	#endif
	
	switch (scanTag) {
		case 0:
			//#if (_HW_SOLUTION_ != _HW_EVB_)
			battAdcParsing(&sTag.battAdc);
			//#endif
			
			//printf("->0\n");
			sp1kADCSarSel(0x00);
			scanTag = 0x01;
			break;
			
		case 1:
			if(flag1627!=0)
			{
				// keyScanDgpioValParsing(&sTag.keyAdcA);
			}
			else
			{
				// keyScanAdcParsing(&sTag.keyAdcA);
			}
			 keyScanAdcParsing(&sTag.keyAdcA);
			
			#if KEY_VALUE_AVEAGE
			if(keepChannelFlag)
			{
				return;
			}
			#endif
			
			//printf("->1\n");
			sp1kADCSarSel(0x02);
			scanTag = 0x02;
			break;
			
		case 2:
			keyScanAdcParsing(&sTag.keyAdcB);
			//printf("->2\n");
			
			#if KEY_VALUE_AVEAGE
			if(keepChannelFlag)
			{
				return;
			}
			#endif
			
			sp1kADCSarSel(0x00);
			scanTag = 0x00;
			break;
		default:
			break;
	}
	
	keyScanGpioParsing(&sTag.keyGpio);
	
	#if FUNC_HOST_TOUCH_PANEL
	keyScanTpParsing();
	#endif

	// 屏幕保护
	#if SUPPORT_PANEL_PROTECT
	if (--ucPanel100MsDleay == 0) {
		ucPanel100MsDleay = 100;		
		osMsgPost(SP1K_MSG_PANEL_PROTECT);
	}
	#endif

	#if SUPPORT_MCU_WTD
	if (--heartbeatSendDelay3Sec == 0) {
		heartbeatSendDelay3Sec = 50;//200;
		appSetWTDHeartbeat(highLowStatus);
		highLowStatus = !highLowStatus;
	}
	#endif


	if (affarflag) {
		if (--afbackfar == 0) {
			affarflag = 0;
			afbackfar = 500;
			
			osMsgPost(SP1K_MSG_SYS_AF_BACK_FAR);
		}
	}
	else {
		afbackfar = 500;
	}
	
	
}

//-----------------------------------------------------------------------------
//keyScanGpioValParsing
//-----------------------------------------------------------------------------
/**
 * @brief		keyScanGpioValParsing
 * @param
 * @retval	SUCCESS
 * @see
 * @author	Phil Lin
 * @since		2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
void keyScanGpioValParsing(UINT8 gpioVal, UINT8 gpioChn, keyUnitGpio_t* pGpio, UINT8 gpioPrsFlag)
{
	UINT16 gpioMsg = SP1K_MSG_NULL;
	UINT8* gpioPreSts = &pGpio->keyAttr[gpioChn].keyStsPrev;

	if (gpioVal != gpioPrsFlag) {
		if (*gpioPreSts == KEY_STATUS_RELEASED) {
			//TODO : idle -> idle : No key action

			return ;
		} else {
			//TODO : Active -> idle : Released

			*gpioPreSts = KEY_STATUS_RELEASED;
			if (*pGpio->keyAttr[gpioChn].pKeyEn & KEY_STATUS_RELEASED) {
				pGpio->keyCtrl.keyValRpt = KEY_SCAN_GPIO_VAL_RPT;
				gpioMsg = pGpio->pKeyMsg[gpioChn].keyMsgRls;
				
				#if SUPPORT_PANEL_PROTECT
				if (!app_get_lcd_backlight_status()) {
					gpioMsg = SP1K_MSG_SYS_OPEN_LCD_BACKLIGHT;
				}
				#endif
			}
		}
	} else if (*gpioPreSts == KEY_STATUS_RELEASED) {
		//TODO : idle -> Active : Pressed
		*gpioPreSts = KEY_STATUS_PRESSED;
		if (*pGpio->keyAttr[gpioChn].pKeyEn & KEY_STATUS_PRESSED) {
			pGpio->keyCtrl.keyValRpt = KEY_SCAN_GPIO_VAL_RPT_START;
			gpioMsg = pGpio->pKeyMsg[gpioChn].keyMsgPrs;
			#if SUPPORT_PANEL_PROTECT
			if (!app_get_lcd_backlight_status()) {
				gpioMsg = SP1K_MSG_NULL;
			}
			#endif
		}	
	} else if (*pGpio->keyAttr[gpioChn].pKeyEn & KEY_STATUS_REPEAT) {
		//TODO : Active -> Active : Repeated
		if  (--(pGpio->keyCtrl.keyValRpt) == 0){
			pGpio->keyCtrl.keyValRpt = KEY_SCAN_GPIO_VAL_RPT;
			gpioMsg = pGpio->pKeyMsg[gpioChn].keyMsgRpt;
			#if SUPPORT_PANEL_PROTECT
			if (!app_get_lcd_backlight_status()) {
				if (gpioMsg != SP1K_MSG_KEY_REPEATE_POWER) {
					gpioMsg = SP1K_MSG_NULL;
				}
			}
			#endif
		}
		
	}
	
	if (gpioMsg != SP1K_MSG_NULL) {		
		osMsgPost(gpioMsg);

		#if SUPPORT_PANEL_PROTECT
		if (app_get_lcd_backlight_status()) {
			osMsgPost(SP1K_MSG_SYS_OPEN_LCD_BACKLIGHT);
		}
		#endif
	}
}

//-----------------------------------------------------------------------------
//keyScanGpioParsing
//-----------------------------------------------------------------------------
/**
 * @brief		keyScanGpioParsing
 * @param
 * @retval	SUCCESS
 * @see
 * @author	Phil Lin
 * @since		2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
static void keyScanGpioParsing(keyUnitGpio_t* pGpio)
{
	UINT8 gpioVal, curKeyVal;
	UINT8 gpioChn;

#if(_HW_SOLUTION_ == _HW_CUSTOM_MODEL_A_|| _HW_SOLUTION_ == _HW_CUSTOM_MODEL_B_)


	gpioChn = 0;	// Power

	if (*pGpio->keyAttr[gpioChn].pKeyEn) {
		HAL_GpioByteDirSet_Intr(HAL_GPIO_BYTE_MA1, 0x40, 0x00);  // seta ma[14] as input.
		gpioVal = HAL_GpioByteInGet(HAL_GPIO_BYTE_MA1, 0x40, 1);

		keyScanGpioValParsing(gpioVal, gpioChn, pGpio, 0x40);
	}

		gpioChn = 1; // S2
		if (*pGpio->keyAttr[gpioChn].pKeyEn) {
			gpioVal = gpioByteInGet(gpioByteIdS2, gpioBitMskS2, 1);
			keyScanGpioValParsing(gpioVal, gpioChn, pGpio, gpioBitMskS2);
		}
	#elif (_HW_SOLUTION_ == _HW_EVB_ || _HW_SOLUTION_ == _HW_TK_MODEL_A_)
		/* GPIO 按键扫描 */
		#if 0
		gpioChn = 0;	// rec gpio 9
		if (*pGpio->keyAttr[gpioChn].pKeyEn) {
			HAL_GpioByteDirSet_Intr(HAL_GPIO_BYTE_GEN1, 0x02, 0x00);
			gpioVal = HAL_GpioByteInGet(HAL_GPIO_BYTE_GEN1, 0x02, 1);
			keyScanGpioValParsing(gpioVal, gpioChn, pGpio, 0x02);
		}
		#endif



		/* DGPIO 按键扫描 */
		if(flag1627==2) {
			gpioVal = G_DgpioValue;
		}
		else if(flag1627==1) {
			gpioVal = XBYTE[REG_Disp_Rsv3];
		}

		gpioChn = 0;	// zoom in dgpio 1
		curKeyVal = (gpioVal & 0x02);
		if (*pGpio->keyAttr[gpioChn].pKeyEn) {
			keyScanGpioValParsing(curKeyVal, gpioChn, pGpio, 0x00);
		}

		gpioChn = 1;	// zoom out dgpio 2
		curKeyVal = (gpioVal & 0x04);
		if (*pGpio->keyAttr[gpioChn].pKeyEn) {
			keyScanGpioValParsing(curKeyVal, gpioChn, pGpio, 0x00);
		}
		
 	// hw solution
	#else
		gpioChn = 0; // S2

	if (*pGpio->keyAttr[gpioChn].pKeyEn) {
		gpioVal = gpioByteInGet(gpioByteIdS2, gpioBitMskS2, 1);
		keyScanGpioValParsing(gpioVal, gpioChn, pGpio, 0x00);
	}
#endif
}

//-----------------------------------------------------------------------------
//keyScanInit
//-----------------------------------------------------------------------------
/**
 * @brief		keyScanInit
 * @param
 * @retval	SUCCESS
 * @see
 * @author	Phil Lin
 * @since		2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
void keyScanInit(void)
{
	scanTag_t* pTag;
	UINT8 i;

	pTag = &sTag;
	if(((pkgChipIdGet()== 0x1627)||(pkgChipIdGet()== 0x1626))&&(HAL_GlobalDGPIOModeGet()!=0x01))
	{
		XBYTE[REG_Disp_VsVdIntEn] |= 0x02;
		G_DgpioValue = 0;
		flag1627 = 2;
	}
	else if(((pkgChipIdGet()== 0x1627)||(pkgChipIdGet()== 0x1626))&&(HAL_GlobalDGPIOModeGet()==0x01))
	{
		XBYTE[REG_Disp_Rsv0] |= 0x04;
		flag1627 = 1;
	}

	for (i = 0; i < SP1K_ADC_BTN_NUM; i++) {
		sKeyEn[i] = 0x00; // set the default key status as disabled
	}

	pTag->keyAdcA.keyActiveIdx = (SP1K_ADC_BTN_A_NUM - 1);
	pTag->keyAdcA.keyAttr.pKeyEn = &sKeyEn[0];
	pTag->keyAdcA.keyAttr.keyNum = SP1K_ADC_BTN_A_NUM;
	pTag->keyAdcA.keyAttr.keyType = KEY_TYPE_ADC_A;
	pTag->keyAdcA.keyCtrl.keyDec = KEY_SCAN_ADC_VAL_DEC;
	pTag->keyAdcA.keyCtrl.keyValRpt = 0; // KEY_SCAN_ADC_VAL_RPT_START;
	pTag->keyAdcA.keyCtrl.keyValPrev = 0; // KEY_SCAN_ADC_B_VAL_IDLE;
	pTag->keyAdcA.keyCtrl.pKeyDev = &keyAdcAValDevTbl[0];
	pTag->keyAdcA.keyCtrl.pKeyVal = &keyAdcAVal[0];
	pTag->keyAdcA.pKeyMsg = &keyMsg[0];

	pTag->keyAdcB.keyActiveIdx = (SP1K_ADC_BTN_B_NUM - 1);
	pTag->keyAdcB.keyAttr.pKeyEn = &sKeyEn[0 + SP1K_ADC_BTN_A_NUM];
	pTag->keyAdcB.keyAttr.keyNum = SP1K_ADC_BTN_B_NUM;
	pTag->keyAdcB.keyAttr.keyType = KEY_TYPE_ADC_B;
	pTag->keyAdcB.keyCtrl.keyDec = KEY_SCAN_ADC_VAL_DEC;
	pTag->keyAdcB.keyCtrl.keyValRpt = 0; // KEY_SCAN_ADC_VAL_RPT_START;
	pTag->keyAdcB.keyCtrl.keyValPrev = 0; // KEY_SCAN_ADC_B_VAL_IDLE;
	pTag->keyAdcB.keyCtrl.pKeyDev = &keyAdcBValDevTbl[0];
	pTag->keyAdcB.keyCtrl.pKeyVal = &keyAdcBVal[0];
	pTag->keyAdcB.pKeyMsg = &keyMsg[0 + SP1K_ADC_BTN_A_NUM];

	for (i = 0; i < SP1K_GPIO_BTN_NUM; i++) {
		pTag->keyGpio.keyAttr[i].pKeyEn = &sKeyEn[0 + SP1K_ADC_BTN_NUM];
		pTag->keyGpio.keyAttr[i].keyStsPrev = KEY_STATUS_RELEASED;
	}
	pTag->keyGpio.keyCtrl.keyDec = 1;
	pTag->keyGpio.keyCtrl.keyValRpt = KEY_SCAN_GPIO_VAL_RPT_START;
	pTag->keyGpio.pKeyMsg = &keyMsg[0 + SP1K_ADC_BTN_NUM];

	#if USE_PKG_DEDICATE_PIN
	#if(_HW_SOLUTION_ == _HW_CUSTOM_MODEL_A_||_HW_SOLUTION_ == _HW_CUSTOM_MODEL_B_)
	gpioPullSelSet(4, 0x02, 0x00);//GPIO12 pull low
	gpioPullEnSet(5, 0x20, 0xFF);
	#else
	if(pkgChipIdGet()== 0x1626)
	{
		//gpioPullSelSet(3,0x40,0x00);
		//gpioPullEnSet(5,0x04,0x00);//GPIO9 pull dis

		//gpioPullSelSet(2,0x40,0x40);
		//gpioPullEnSet(4,0x04,0x04);//GPIO1 pull high

		//gpioPullSelSet(2,0x80,0x00);
		//gpioPullEnSet(4,0x08,0x00);//GPIO2 pull high
	}
	else
	{
		//gpioPullSelSet(3,0x40,0x40);
		//gpioPullEnSet(5,0x04,0x04);//pull high
	}
	#endif
	pkgDedicateGpioCfg(PKG_DEDICATE_PIN_S2, &gpioByteIdS2, &gpioBitMskS2, &gpiopolarityS2, 0);
	HAL_GpioByteFuncSet(gpioByteIdS2, gpioBitMskS2, 0xFF);
	#endif
}

//-----------------------------------------------------------------------------
//battDetectCfg
//-----------------------------------------------------------------------------
/**
 * @brief		battDetectCfg
 * @param	UINT8 battLvlTol, UINT16* pBattAdcVal, UINT16 battAdvValDev
 * @retval	NONE
 * @see
 * @author	Phil Lin
 * @since		2008-07-01
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT8 battDetectCfg(UINT8 battLvlTol, UINT16* pBattAdcVal, UINT16 battAdvValDev)
{
	scanTag_t* pTag;

	/*Param check*/
	if (!battLvlTol || pBattAdcVal == NULL) {
		return FAIL;
	}

	pTag = &sTag;

	pTag->battAdc.battLvlTol = battLvlTol;
	pTag->battAdc.pbattAdcVal = pBattAdcVal;
	pTag->battAdc.battAdcDev = battAdvValDev;

	return SUCCESS;
}

//-----------------------------------------------------------------------------
//battDetectInit
//-----------------------------------------------------------------------------
/**
 * @brief		battDetectInit
 * @param	NONE
 * @retval	battery  level
 * @see
 * @author	Phil Lin
 * @since		2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT8 battDetectInit(void)
{
	scanTag_t* pTag;
	UINT16 adcVal;
	UINT8 i;

	pTag = &sTag;
	adcVal=sp1kADCValueSnap();
#if (_HW_SOLUTION_ != _HW_EVB_)
	//ready to check power key
	sp1kADCSarSel(0x02);
#endif

	/*the default is lowest level*/
	pTag->battAdc.battLvl  = pTag->battAdc.battLvlTol;

	/*scan the adc value, if no matched, keep the lowest battery level*/
	for (i = 0; i < pTag->battAdc.battLvlTol; i++) {
		if (adcVal >= pTag->battAdc.pbattAdcVal[i]) {
			pTag->battAdc.battLvl = i;
			break;
		}
	}

	pTag->battAdc.battDec = 0;
	pTag->battAdc.battDetectEn = 0;

	/*if usb plug in, force battery full*/
	if (sp1kUSB_PlugGet() == USB_PLUG_IN) {
	// if (!gpioByteInGet(GPIO_BYTE_DISP0, 0x02, 1)) {
		pTag->battAdc.battLvl = 0;
	}

	//#if (_HW_SOLUTION_ == _HW_EVB_) //2008-7-22 mantis #28665
	//pTag->battAdc.battLvl = 0;
	//#endif
	
	return pTag->battAdc.battLvl;
}

//-----------------------------------------------------------------------------
//sp1kBattEnable
//-----------------------------------------------------------------------------
/**
 * @brief		sp1kBattEnable
 * @param	enable / disable
 * @retval	NONE
 * @see
 * @author	Phil Lin
 * @since		2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
void sp1kBattEnable(UINT8 enable)
{
	scanTag_t* pTag;

	pTag = &sTag;

	if (enable) {
		pTag->battAdc.battDetectEn = 1;
	} else {
		pTag->battAdc.battDetectEn = 0;
	}
}

//-----------------------------------------------------------------------------
//battAdcParsing
//-----------------------------------------------------------------------------
/**
 * @brief		battAdcParsing
 * @param
 * @retval	SUCCESS
 * @see
 * @author	Phil Lin
 * @since		2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
static void battAdcParsing(battUnitAdc_t* pAdc)
{
	UINT16 adcVal;
	UINT8 battLv;
	static UINT16 pwrOffRetry = 1;
	UINT16 adcGap;
	UINT8 i;
	UINT8 adcTot;

	static UINT8 pr_delay = 30;

	if (!pAdc->battDetectEn) {
		return;
	}

	/*capture the adc value*/
	adcVal = sp1kADCValueSnapIntr();
	
	#if 0
	if (--pr_delay == 0) {
		pr_delay = 30;
		printf("batter val(%u)\n", adcVal);
	}
	#endif

	if (adcVal >= 300) {
		return;
	}
	
	adcGap = pAdc->battAdcDev;
	adcTot = pAdc->battLvlTol;

	battLv = -1;
	for(i = 0; i <= adcTot; i++) {
		if (!i) {
			/* when i == 0;*/
			if (adcVal >= pAdc->pbattAdcVal[i]) {
				battLv = i;
				break;
			}
		} else if (i == adcTot) {
			/*when scan the last value*/
			if (adcVal < pAdc->pbattAdcVal[i - 1] - adcGap) {
				battLv = adcTot;
				break;
			}
		} else {
			if (adcVal >= pAdc->pbattAdcVal[i] && adcVal < pAdc->pbattAdcVal[i - 1] - adcGap) {
				battLv = i;
				break;
			}
		}
	}

	
	if (battLv == -1) {
		/*adc value is in gap*/
		//printf("gap!\n");
		return;
	}

	/*If power is in low battery, need be send SP1K_MSG_BAT_LV_UPDATE by cycle */
	if (battLv == adcTot) {

		/*debounds*/
		if (++pAdc->battDec < POWER_BAT_DEC) {
			return;
		}

		/*power off retry*/
		if (!(--pwrOffRetry)) {

			pAdc->battLvl = battLv;
			if (pAdc->battDec) {
				/*restore battdec*/
				--pAdc->battDec;
			}
			pwrOffRetry = BATT_LOW_POWER_OFF_RETRY;
			osMsgPost(SP1K_MSG_BAT_LV_UPDATE);
		}
	} else if (battLv != pAdc->battLvl) {
		/*debounds*/
		if (++pAdc->battDec < POWER_BAT_DEC+10) {
			return;
		} else {
			/*battery level changed*/
			pAdc->battDec = 0;
			pAdc->battLvl = battLv;
			//printf("->(%bu)\n", pAdc->battLvl);
			osMsgPost(SP1K_MSG_BAT_LV_UPDATE);
		}
	} else {
		/*battery level not changed*/
		#ifdef DBG_KEYSCAN
		//#if 1
		if (pAdc->battDec) {
			//printf("dec(%bu)\n", pAdc->battDec);
			pAdc->battDec = 0;
		}
		#else
		pAdc->battDec = 0;
		#endif
	}
}


void sp1kOsdNumberShow(UINT16 num,UINT8 x,UINT8 y,UINT8 fillbit,UINT8 attrib);


UINT8 adca_sample_count=0;
UINT8 adcb_sample_count=0;
UINT16 adca_sample[3];
UINT16 adcb_sample[3];

#define ADC_SAMPLE_ERR_LIMIT 0x35
#define ADC_DIFF(a,b) (a>b?a-b:b-a)

UINT16 adc_filter_noise(UINT16 value,UINT8 channel)
{
	UINT16 diff;
	if(channel==1)
	{
		adca_sample[adca_sample_count]=value;
		adca_sample_count++;
		
		if(adca_sample_count>=3)
		{
			adca_sample_count=0;
			
			diff=ADC_DIFF(adca_sample[0],adca_sample[1]);
			if(diff>ADC_SAMPLE_ERR_LIMIT)
				return -1;
			
			diff=ADC_DIFF(adca_sample[0],adca_sample[2]);
			if(diff>ADC_SAMPLE_ERR_LIMIT)
				return -1;
			
			diff=ADC_DIFF(adca_sample[1],adca_sample[2]);
			if(diff>ADC_SAMPLE_ERR_LIMIT)
				return -1;
			
			return  (adca_sample[2]+adca_sample[1])>>1;	 //简化计算仅做2次平均
		}
		else
			return -1;
	}
	else if(channel==2)
	{
		adcb_sample[adcb_sample_count]=value;
		adcb_sample_count++;
		
		if(adcb_sample_count>=3)
		{
			adcb_sample_count=0;
			
			diff=ADC_DIFF(adcb_sample[0],adcb_sample[1]);
			if(diff>ADC_SAMPLE_ERR_LIMIT)
				return -1;
			
			diff=ADC_DIFF(adcb_sample[0],adcb_sample[2]);
			if(diff>ADC_SAMPLE_ERR_LIMIT)
				return -1;
			
			diff=ADC_DIFF(adcb_sample[1],adcb_sample[2]);
			if(diff>ADC_SAMPLE_ERR_LIMIT)
				return -1;
			
			return  (adcb_sample[2]+adcb_sample[1])>>1;	 //
		}
		else
			return -1;
	}
}


//-----------------------------------------------------------------------------
//keyScanAdcParsing
//-----------------------------------------------------------------------------
/**
 * @brief		keyScanAdcParsing
 * @param
 * @retval	SUCCESS
 * @see
 * @author	Phil Lin
 * @since		2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
static void keyScanAdcParsing(keyUnitAdc_t* pAdc)
{
	UINT16 adcVal;
	UINT16 msg = SP1K_MSG_NULL;

	 static UINT8 ucCountb = 20,ucCounta = 20;
	
	adcVal = sp1kADCValueSnapIntr();

	#if 1
	adcVal=adc_filter_noise(adcVal,pAdc->keyAttr.keyType);
	if(adcVal==0xffff)
		return;
	#endif

	if (KEY_TYPE_ADC_B == pAdc->keyAttr.keyType) {
		adcVal = 1023 - adcVal;

		#if 1
		if (--ucCountb == 0) {
			ucCountb = 20;
			printf("adc bbbbb val : %d\n", adcVal);
		}
		#endif
	}else{

		#if 0
		if (--ucCounta == 0) {
			ucCounta = 20;
			printf("adc aaaaa val : %d\n", adcVal);
		}
		#endif
	}
	

	//TODO: Active -> Active : Key repeated
	if ((keyScanAdcIdleChk(adcVal, pAdc) == TRUE) && pAdc->keyCtrl.keyValRpt) {
		//printf("press 2 adcval : %d\n", adcVal);
		if (pAdc->keyAttr.pKeyEn[pAdc->keyActiveIdx] & KEY_STATUS_REPEAT) {
			if	(--(pAdc->keyCtrl.keyValRpt) == 0) {
				pAdc->keyCtrl.keyValRpt = KEY_SCAN_ADC_VAL_RPT;
				
				if (pAdc->pKeyMsg[pAdc->keyActiveIdx].keyMsgRpt != SP1K_MSG_NULL) {
					msg = pAdc->pKeyMsg[pAdc->keyActiveIdx].keyMsgRpt;

					#if SUPPORT_PANEL_PROTECT
					if (!app_get_lcd_backlight_status()) {
						if (msg != SP1K_MSG_KEY_REPEATE_POWER) {
							msg = SP1K_MSG_NULL;
						}
					}
					#endif
				}
				
				goto EXIT;
			}
		}
	}
	
	if (keyScanAdcIdleChk(pAdc->keyCtrl.keyValPrev, pAdc) == TRUE) {
		//printf("press 3 adcval : %d\n", adcVal);
		if (keyScanAdcIdleChk(adcVal, pAdc) == TRUE) {
			//TODO: Idle -> Idle : No key action
		} else {
			pAdc->keyCtrl.keyValRpt = 0;
			
			if (keyScanAdcValParsing(pAdc->keyCtrl.keyValPrev, pAdc, &pAdc->keyActiveIdx) != SUCCESS) {
				//No matched adc value
				adcVal = 0;
				goto EXIT;
			}
			
			//TODO: Active -> Idle : Key released
			if (pAdc->keyAttr.pKeyEn[pAdc->keyActiveIdx] & KEY_STATUS_RELEASED) {
				msg = pAdc->pKeyMsg[pAdc->keyActiveIdx].keyMsgRls;
				pAdc->keyActiveIdx = -1; //key idle

				#if SUPPORT_PANEL_PROTECT
				if (!app_get_lcd_backlight_status()) {
					msg = SP1K_MSG_SYS_OPEN_LCD_BACKLIGHT;
				}
				#endif
			}
		}

		goto EXIT;
	} else if (keyScanAdcIdleChk(adcVal, pAdc) == TRUE) {
		//printf("press 1 adcval : %d\n", adcVal);
		
		//TODO: Idle  -> Active : Key pressed
		if (keyScanAdcValParsing(adcVal, pAdc, &pAdc->keyActiveIdx) != SUCCESS) {
			//No matched adc value
			adcVal = 0;
			goto EXIT;
		}
		if (pAdc->keyAttr.pKeyEn[pAdc->keyActiveIdx] & KEY_STATUS_PRESSED) {
			pAdc->keyCtrl.keyValRpt = KEY_SCAN_ADC_VAL_RPT_START;
			msg = pAdc->pKeyMsg[pAdc->keyActiveIdx].keyMsgPrs;

			#if SUPPORT_PANEL_PROTECT
			if (!app_get_lcd_backlight_status()) {
				msg = SP1K_MSG_NULL;
			}
			#endif
		}
	}

EXIT:
	pAdc->keyCtrl.keyValPrev = adcVal;
	
	if (msg != SP1K_MSG_NULL) {
		if((msg<SP1K_MSG_KEY_RELEASE_MAX)&&(msg>SP1K_MSG_KEY_PRESS_MAX))
			osMsgPostForce( msg);
		else
			osMsgPost(msg);

		#if SUPPORT_PANEL_PROTECT
		if (app_get_lcd_backlight_status()) {
			osMsgPost(SP1K_MSG_SYS_OPEN_LCD_BACKLIGHT);
		}
		#endif
	}
}


//-----------------------------------------------------------------------------
//keyScanAdcValParsing
//-----------------------------------------------------------------------------
/**
 * @brief		keyScanAdcValParsing
 * @param
 * @retval	SUCCESS
 * @see
 * @author	Phil Lin
 * @since		2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
static UINT8 keyScanAdcValParsing(UINT16 AdcValue, keyUnitAdc_t* pAdc, UINT8* AdcIdx)
{
	UINT8 i;
	UINT8 adcNum = pAdc->keyAttr.keyNum;
	UINT16* pAdcVal = &pAdc->keyCtrl.pKeyVal[0];
	UINT16* pAdcDev = &pAdc->keyCtrl.pKeyDev[0];

	if (pAdc == NULL || AdcIdx == NULL) {
		return FAIL;
	}

	for(i = 0; i < adcNum; i++) {
		if (AdcValue < (*pAdcVal + *pAdcDev)) {
			if (((*pAdcVal > *pAdcDev) && (AdcValue > (*pAdcVal - *pAdcDev))) || \
				((*pAdcVal <= *pAdcDev) && (AdcValue <= (*pAdcVal + *pAdcDev)))) {
				//printf("idx(%bu)\n", i);
				pAdc->keyActiveIdx = i;
				*AdcIdx = i;
				return SUCCESS;
			}
		}
		pAdcVal++;
		pAdcDev++;
	}

	return FAIL;
}

//-----------------------------------------------------------------------------
//keyScanAdcIdleChk
//-----------------------------------------------------------------------------
/**
 * @brief		keyScanAdcIdleChk
 * @param	AdcValue,  pAdc
 * @retval	SUCCESS
 * @see
 * @author	Phil Lin
 * @since		2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
static UINT8 keyScanAdcIdleChk(UINT16 AdcValue, keyUnitAdc_t* pAdc)
{
#if (_HW_SOLUTION_ == _HW_RDK_A_ )
	if ((pAdc->keyAttr.keyType == KEY_TYPE_ADC_A && AdcValue <= KEY_SCAN_ADC_A_VAL_IDLE) ||
	(pAdc->keyAttr.keyType == KEY_TYPE_ADC_B && AdcValue <= KEY_SCAN_ADC_B_VAL_IDLE)

#elif (_HW_SOLUTION_ == _HW_CUSTOM_MODEL_A_)
		if ((pAdc->keyAttr.keyType == KEY_TYPE_ADC_A && AdcValue <= KEY_SCAN_ADC_A_VAL_IDLE) ||
		(pAdc->keyAttr.keyType == KEY_TYPE_ADC_B && AdcValue <= KEY_SCAN_ADC_B_VAL_IDLE)
#elif (_HW_SOLUTION_ == _HW_CUSTOM_MODEL_B_)
		if ((pAdc->keyAttr.keyType == KEY_TYPE_ADC_A && AdcValue <= KEY_SCAN_ADC_A_VAL_IDLE) ||
		(pAdc->keyAttr.keyType == KEY_TYPE_ADC_B && AdcValue <= KEY_SCAN_ADC_B_VAL_IDLE)
#elif(_HW_SOLUTION_ == _HW_EVB_ || _HW_SOLUTION_ == _HW_TK_MODEL_A_)
	if ((pAdc->keyAttr.keyType == KEY_TYPE_ADC_A && AdcValue >= KEY_SCAN_ADC_A_VAL_IDLE) ||
		(pAdc->keyAttr.keyType == KEY_TYPE_ADC_B && AdcValue >= KEY_SCAN_ADC_B_VAL_IDLE)
#elif(_HW_SOLUTION_ == _HW_TK_MODEL_B_)
if ((pAdc->keyAttr.keyType == KEY_TYPE_ADC_A && AdcValue >= KEY_SCAN_ADC_A_VAL_IDLE) ||
	(pAdc->keyAttr.keyType == KEY_TYPE_ADC_B && AdcValue <= KEY_SCAN_ADC_B_VAL_IDLE)
#endif

	) {
		return TRUE;
	} else {
		return FALSE;
	}
}

static UINT8 cnt = 0;
static void keyScanDgpioValParsing(keyUnitAdc_t* pAdc)
{
	UINT16 msg = SP1K_MSG_NULL;
	UINT8 curKeyVal = 0xFF;
	UINT8 keyValue;

	if(flag1627==2) {
		keyValue = G_DgpioValue;
	}
	else if(flag1627==1) {
		keyValue = XBYTE[REG_Disp_Rsv3];
	}

	/*cnt++;
	if( 150<=cnt ){
		printf(" 0x%bx ",keyValue);
		cnt = 0;
	}*/

	if(((pkgChipIdGet()== 0x1627)||(pkgChipIdGet()== 0x1626)))
	{
		keyValue >>= 2;
		switch(keyValue&0x3F)
		{
			case 0x3D: // WIDE
				curKeyVal = 5;
				break;
			case 0x3E: // TELE
				curKeyVal = 4;
				break;
			case 0x3B: // UP
				curKeyVal = 3;
				break;
			case 0x2F: // LEFT
				curKeyVal = 1;
				break;
			case 0x1F: // RIGHT
				curKeyVal = 0;
				break;
			case 0x37: // DOWN
				curKeyVal = 2;
				break;
			case 0x3F:
			default:
				if( prevKeyVal < 3 ) {
					if (pAdc->keyAttr.pKeyEn[pAdc->keyActiveIdx]& KEY_STATUS_RELEASED ){
						msg = pAdc->pKeyMsg[(UINT8)(pAdc->keyCtrl.keyValPrev)].keyMsgRls;
					}
				}
			break;
		}
	}
	else
	{
		if ( !(keyValue & 0x02) ) {   // dgpio1 left
			curKeyVal = 1;
		} else if ( !(keyValue & 0x04) ) {//dgpio2 right
			curKeyVal = 2;
		} else {
			if( prevKeyVal < 3 ) {
				if (pAdc->keyAttr.pKeyEn[pAdc->keyActiveIdx]& KEY_STATUS_RELEASED ){
					msg = pAdc->pKeyMsg[(UINT8)(pAdc->keyCtrl.keyValPrev)].keyMsgRls;
				}
			}
		}
	}

	if( (prevKeyVal==curKeyVal) && (curKeyVal!=0xFF) )
	{
		if (pAdc->keyAttr.pKeyEn[pAdc->keyActiveIdx]& KEY_STATUS_REPEAT){
			if  (--pAdc->keyCtrl.keyValRpt == 0)
			{
				pAdc->keyCtrl.keyValRpt = KEY_SCAN_ADC_VAL_RPT;
				msg = pAdc->pKeyMsg[curKeyVal].keyMsgRpt;

				#if SUPPORT_PANEL_PROTECT
				if (!app_get_lcd_backlight_status()) {
					if (msg != SP1K_MSG_KEY_REPEATE_POWER) {
						msg = SP1K_MSG_NULL;
					}
				}
				#endif
			}
		}
	}
	else if( curKeyVal != 0xff )
	{
		if (pAdc->keyAttr.pKeyEn[pAdc->keyActiveIdx]& KEY_STATUS_PRESSED){
			msg = pAdc->pKeyMsg[curKeyVal].keyMsgPrs;
			pAdc->keyCtrl.keyValRpt = KEY_SCAN_ADC_VAL_RPT_START * 3;
			
			#if SUPPORT_PANEL_PROTECT
			if (!app_get_lcd_backlight_status()) {
				msg = SP1K_MSG_NULL;
			}
			#endif
		}
	}

	prevKeyVal = curKeyVal;
	pAdc->keyCtrl.keyValPrev = curKeyVal;

	if (msg != SP1K_MSG_NULL) {
		if((msg<SP1K_MSG_KEY_RELEASE_MAX)&&(msg>SP1K_MSG_KEY_PRESS_MAX))
			osMsgPostForce(msg);
		else
			osMsgPost(msg);
	}

}
//-----------------------------------------------------------------------------
//sp1kBtnCfg
//-----------------------------------------------------------------------------
/**
 * @brief		button configure
 * @param	type: Adc A/ Adc B/ Gpio,
			BitMask:
			Enable:
			Bit 0:key press enable
			Bit 1: key release enable
			Bit 2: key repeat enable
 * @retval	SUCCESS
 * @see
 * @author	Phil Lin
 * @since		2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT8 sp1kBtnCfg(keyType_t BtnType, UINT16 BitMask, UINT8 Enable)
{
	UINT8 keyNum;
	UINT8* pKeyEn;
	UINT8 i;

	switch (BtnType) {
		case KEY_TYPE_ADC_A:
			keyNum = SP1K_ADC_BTN_A_NUM;
			pKeyEn = &sKeyEn[0];
			break;
		case KEY_TYPE_ADC_B:
			keyNum = SP1K_ADC_BTN_B_NUM;
			pKeyEn = &sKeyEn[0 + SP1K_ADC_BTN_A_NUM];
			break;
		case KEY_TYPE_GPIO:
			keyNum = SP1K_GPIO_BTN_NUM;
			pKeyEn = &sKeyEn[0 + SP1K_ADC_BTN_NUM];
			break;
		default:
			return FAIL;
	}
/**/
	for (i = 0; i < keyNum; i++) {
		if ((BitMask >> i) & 0x01) {
			*pKeyEn = Enable;
		}
		pKeyEn++;
	}
	//printf("En flag %u\n", *pKeyEn);
	return SUCCESS;
}


UINT8 sp1kBtnCfgIntr(keyType_t BtnType, UINT16 BitMask, UINT8 Enable)
{
	UINT8 keyNum;
	UINT8* pKeyEn;
	UINT8 i;

	return SUCCESS;

	switch (BtnType) {
		case KEY_TYPE_ADC_A:
			keyNum = SP1K_ADC_BTN_A_NUM;
			pKeyEn = &sKeyEn[0];
			break;
		case KEY_TYPE_ADC_B:
			keyNum = SP1K_ADC_BTN_B_NUM;
			pKeyEn = &sKeyEn[0 + SP1K_ADC_BTN_A_NUM];
			break;
		case KEY_TYPE_GPIO:
			keyNum = SP1K_GPIO_BTN_NUM;
			pKeyEn = &sKeyEn[0 + SP1K_ADC_BTN_NUM];
			break;
		default:
			return FAIL;
	}
/**/
	for (i = 0; i < keyNum; i++) {
		if ((BitMask >> i) & 0x01) {
			*pKeyEn = Enable;
		}
		pKeyEn++;
	}
	//printf("En flag %u\n", *pKeyEn);
	return SUCCESS;
}



//-----------------------------------------------------------------------------
//sp1kAdcHdlGet
//-----------------------------------------------------------------------------
/**
 * @brief		sp1kAdcHdlGet
 * @param
 * @retval	SUCCESS
 * @see
 * @author	Phil Lin
 * @since		2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
void sp1kAdcHdlGet(const scanTag_t** pAdcHdl)
{
	*pAdcHdl = (const scanTag_t*)&sTag;
}

//-----------------------------------------------------------------------------
//sp1kBtnChk
//-----------------------------------------------------------------------------
/**
 * @brief		check the status of specify button
 * @param
 * @retval	button Status
 			1:KEY_STATUS_PRESSED
 			2:KEY_STATUS_RELEASED
 * @see
 * @author	Phil Lin
 * @since		2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT8 sp1kBtnChk(UINT8 button)
{
	UINT8 btnSts;
	UINT8 gpioVal;
	return KEY_STATUS_RELEASED;

#if(_HW_SOLUTION_ == _HW_CUSTOM_MODEL_A_||_HW_SOLUTION_ == _HW_CUSTOM_MODEL_B_)
	if (button == SP1K_BTN_S2) {
		#if USE_PKG_DEDICATE_PIN
		pkgDedicateGpioCfg(PKG_DEDICATE_PIN_S2, &gpioByteIdS2, &gpioBitMskS2, &gpiopolarityS2, 0);
		if(pkgChipIdGet()== 0x1626)
		{
			//gpioPullSelSet(3,0x40,0x00);
			//gpioPullEnSet(5,0x04,0x00);//GPIO9 pull dis

			gpioPullSelSet(2,0x40,0x40);
			gpioPullEnSet(4,0x04,0x04);//GPIO1 pull high
		}
		else
		{
			gpioPullSelSet(3,0x40,0x40);
			gpioPullEnSet(5,0x04,0x04);//pull high
		}
		gpioVal = gpioByteInGet(gpioByteIdS2, gpioBitMskS2, gpiopolarityS2);
		#endif
		if (gpioVal) {
			//CPrintf("Press");
			btnSts = KEY_STATUS_PRESSED;
		} else {
			//CPrintf("Release");
			btnSts = KEY_STATUS_RELEASED;
		}
	}
#else

	if (button == SP1K_BTN_S2) {
		#if USE_PKG_DEDICATE_PIN
		pkgDedicateGpioCfg(PKG_DEDICATE_PIN_S2, &gpioByteIdS2, &gpioBitMskS2, &gpiopolarityS2, 0);
		if(pkgChipIdGet()== 0x1626)
		{
			//gpioPullSelSet(3,0x40,0x00);
			//gpioPullEnSet(5,0x04,0x00);//GPIO9 pull dis

			gpioPullSelSet(2,0x40,0x40);
			gpioPullEnSet(4,0x04,0x04);//GPIO1 pull high
		}
		else
		{
			//gpioPullSelSet(3,0x40,0x40);
			//gpioPullEnSet(5,0x04,0x04);//pull high
		}
		gpioVal = gpioByteInGet(gpioByteIdS2, gpioBitMskS2, 1);
		#endif
		if (!gpioVal) {
			//CPrintf("Release");
			btnSts = KEY_STATUS_RELEASED;
		} else {
			//CPrintf("Press");
			btnSts = KEY_STATUS_PRESSED;
		}
	}

#endif

	else if (button == SP1K_BTN_POWER) {

#if (_HW_SOLUTION_ == _HW_EVB_ || _HW_SOLUTION_ == _HW_TK_MODEL_A_ )
		gpioByteDirSet(GPIO_BYTE_GEN1, 0x04, 0x00);
		if ( !gpioByteInGet(GPIO_BYTE_GEN1, 0x04, 1) ) {
			btnSts = KEY_STATUS_RELEASED;
		} else {
			btnSts = KEY_STATUS_PRESSED;
		}

#elif (_HW_SOLUTION_ == _HW_CUSTOM_MODEL_A_||_HW_SOLUTION_ == _HW_CUSTOM_MODEL_B_)
{

		gpioByteDirSet(GPIO_BYTE_MA1, 0x40, 0x00);
		if ( !gpioByteInGet(GPIO_BYTE_MA1, 0x40, 1) ) {
			btnSts = KEY_STATUS_RELEASED;
		} else {
			btnSts = KEY_STATUS_PRESSED;
		}

}

#elif (_HW_SOLUTION_ == _HW_TK_MODEL_B_ || _HW_SOLUTION_ == _HW_RDK_A_)
{
		UINT16 value;
		sp1kADCSarSel(0x02);
		sp1kHalWait(5);
		value = sp1kADCValueSnap();
		if((value > (SP1K_ADC_BTN_B_S4 + SP1K_ADC_BTN_DEV_B_S4)) || \
			(value < (SP1K_ADC_BTN_B_S4 - SP1K_ADC_BTN_DEV_B_S4))) {
			btnSts = KEY_STATUS_RELEASED;
		} else {
			btnSts = KEY_STATUS_PRESSED;
		}
		sp1kADCSarSel(0x00);

}
#endif
	}

#if (_HW_SOLUTION_ == _HW_CUSTOM_MODEL_A_||_HW_SOLUTION_ == _HW_CUSTOM_MODEL_B_)
	else if(button == SP1K_BTN_MODE)
	{
		UINT16 value;
		sp1kADCSarSel(0x02);
		sp1kHalWait(5);
		value = sp1kADCValueSnap();
		if((value > (SP1K_ADC_BTN_B_S2 + SP1K_ADC_BTN_DEV_B_S2)) || \
			(value < (SP1K_ADC_BTN_B_S2 - SP1K_ADC_BTN_DEV_B_S2))) {
			btnSts = KEY_STATUS_RELEASED;
		} else {
			btnSts = KEY_STATUS_PRESSED;
		}
		sp1kADCSarSel(0x00);

	}


#endif

	return btnSts;
}

#if FUNC_HOST_TOUCH_PANEL

/**
 * @fn        void keyScanTpParsing(void)
 * @brief     keyScanTpParsing
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-12-23
 * @todo      N/A
 * @bug       N/A
*/
void keyScanTpParsing(void)
{
	static guiObj_t *pObjAct;
	static UINT16 tpPenRptVal = TP_PEN_RPT_START;
	guiObj_t *pObj = NULL;

	if (tpCurrStsGet() == TP_PENDOWN) {
		guiObjActHdlGet(&pObj);
		if ((pObj != NULL)&&(pObj->pObjExt && pObj->pObjExt->penRepeateEn)) {
			if (pObjAct == NULL) {
				pObjAct = pObj;
			}
			if (--tpPenRptVal == 0) {
				tpPenRptVal = TP_PEN_RPT;
				guiObjActHdlGet(&pObj);
				if (pObj == pObjAct) {
					if (pObj->pObjExt && pObj->pObjExt->msg) {
						osMsgPost(pObj->pObjExt->msg);
					}
				}
			}
  		}
	} else {
		tpPenRptVal = TP_PEN_RPT_START;
		pObjAct = NULL;
	}

}
#endif
