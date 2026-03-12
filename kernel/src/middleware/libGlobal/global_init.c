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
 * @file      global_init.c
 * @brief     Brief
 * @author    Phil Lin
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#define __FILE __FILE_ID_GLOBAL_INIT__
#include "general.h"
#define SYS_INIT_INTERNAL // for system initialization internal use

#include "hal_global.h"
#include "hal_dram.h"
#include "hal_usb.h"
#include "hal_device.h"

#include "reg_def.h"
#include "global_init.h"
#include "os_api.h"
#include "os_custom.h"
#include "os_msg.h"
#include "cmd.h"
#include "func_init_def.h"
#include "timer.h"
#include "solution.h"
#include "initio.h"
#include "gpio.h"
#include "ctlsetup.h"
#include "bulkout.h"
#include "usbmsdc2.h"
#include "main.h"
#include "uiflow.h"
#include "cardopt.h"
#include "class.h"
#include "dbg_file_list.h"
#include "dbg_def.h"

#include "sp1k_gpio_api.h"
#include "sp1k_aud_api.h"
#include "sp1k_disk_api.h"
#include "sp1k_util_api.h"
#include "sp1k_hal_api.h"
#include "sp1k_zoom_api.h"
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
UINT8 xdata bUsbPwr;
xdata UINT8  G_UI_USBConnect;
xdata UINT16 G_UIStatus;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern SYS_INIT_DCL pFuncInit code funcInitArray[];
extern SYS_INIT_DCL pFuncInit code funcInitMain;

extern UINT8 xdata G_ucPktStorIntrf;
extern UINT8 xdata G_MLUN_CardSupport;
extern xdata UINT8 G_USBAudioSetInterface;
extern UINT8 HAL_TpPreCfg(void);
extern UINT8 zoomDevInit(void);
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/**
 * @fn        void uartInit(void)
 * @brief     uartInit
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-15
 * @todo      N/A
 * @bug       N/A
*/
static void
uartInit(
	void
)
{
	/* The baudrate set in "startup.a51" */

	HAL_GlobalUartSet(1);				/* Pin mux selection for RX/TX */

	globalInitPrintf("\niCatLite\n");	/* First debug message in power on process */
}

/**
 * @fn        void cpuInit(void)
 * @brief     cpuInit
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-15
 * @todo      N/A
 * @bug       N/A
*/
static void
cpuInit(
	void
)
{
    TCON = 0x40;                    //TCON:  IT0=0 INT0 negative edge trigger
                                    //       IE0=0 INT0 interrupt flag
                                    //       IT1=0 INT1 negative level trigger
                                    //       IE1=0 INT1 interrupt flag
                                    //       TR0=0 TIMER0 disable
                                    //       TF0=0 TIMER0 overflow flag
                                    //       TR1=1 TIMER1 enable
                                    //       TF1=0 TIMER1 overflow flag

    IP = 0x03;                      //IP:    PX0=1 INT0 priority level
                                    //       PT0=1 TIMER0 priority level
                                    //       PX1=0 INT1 priority level
                                    //       PT1=0 TIMER1 priority level
                                    //       PS=0  Serial Port priority level
                                    //       PT2=0 Timer2 priority level

    IE = 0x81;                      //IE:    EX0=1 INT0 interrupt enable
                                    //       ET0=0 TIMER0 interrupt disable
                                    //       EX1=0 INT1 interrupt disable
                                    //       ET1=0 TIMER1 interrupt disable
                                    //       ES=0  Serial Port interrupt disable
                                    //       ET2=0 TIMER2 interrupt disable
                                    //       EA=1  All interrupts enable

    TMOD = 0x11;                    //TMOD:  Timer0 M1M0=1 (Mode1=16-bit counter)
                                    //       Timer0 C/T~=0 (Timer)
                                    //       Timer0 GATE=0
                                    //       Timer1 M1M0=2 (Mode2=8-bit counter)
                                    //       Timer1 C/T~=0 (Timer)
                                    //       Timer1 GATE=0

	IT1 = 1; 						// it determines the EX1 trigger method

	TH1 = K_Timer1IntervalHi;
	TL1 = K_Timer1IntervalLo;
}

/**
 * @fn        void dramInit(void)
 * @brief     dramInit
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-15
 * @todo      N/A
 * @bug       N/A
*/
static void
dramInit(
	void
)
{
	/* Dram access priority setting */
	XBYTE[REG_Dram_Master0b_Sel] = 0x00;	/* set CDSP priority more than display */

	XBYTE[REG_Dram_SdckMode] &= ~0x40;/*ma14 output enable,must set when dram is 16*16*/
	/* Code sentry setting */ //Phil move to "sysMemAddrUpdate"
	/* protect 1 word firstly, then for all code area after system memeory initial */
	HAL_DramCodeSentry(0x00, 0x00, 0x00);

	/* watch dog timer initial */
	dbgWdtCfg(1, WDT_PROC_DBG_ALL, WDT_TIME_OUT);

	/* global timer initial */
	HAL_GlobalTimerInit();
}

/**
 * @fn        void gpioInit(void)
 * @brief     gpioInit
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-15
 * @todo      N/A
 * @bug       N/A
*/
static void
gpioInit(
	void
)
{
	XBYTE[0x201a] &= 0xf0;	 /* Mux-pin function : GPIO 28~31 */

	XBYTE[0x2474] = 0x01;
	
	if((pkgChipIdGet()== 0x1627)||(pkgChipIdGet()== 0x1626))
	{
		XBYTE[0x2009] = 0x00;//only for 1627
		XBYTE[0x2049] = 0xA9;
		XBYTE[0x204A] = 0x56;
		XBYTE[0x2060] = 0x56;//mantis for 42697
	}

	#ifndef LIB_RELEASE
		#if(_HW_SOLUTION_ == _HW_EVB_)
			XBYTE[0x2048] = 0x75;//only increasing driving for EVB
		#endif
	#endif

	#if defined(AMCAP_OPTION) || defined(ICAT_OPTION)
	//#ifdef ICAT_OPTION
		bUsbPwr = 0;

		//if (prjDbgMode) {
		//	bUsbPwr = !bUsbPwr;
		//}
	#else

		#if(_HW_SOLUTION_ == _HW_CUSTOM_MODEL_A_||_HW_SOLUTION_ == _HW_CUSTOM_MODEL_B_)
			if (sp1kBtnChk(SP1K_BTN_MODE) != KEY_STATUS_PRESSED) {
				bUsbPwr = 0;
			} else {
				bUsbPwr = 1;
			}
		#else
			if (sp1kBtnChk(SP1K_BTN_S2) != KEY_STATUS_PRESSED) {
				bUsbPwr = 0;
			} else {
				bUsbPwr = 1;
			}
		#endif

		if (prjDbgMode) {
			bUsbPwr = !bUsbPwr;
		}
	#endif //ICAT_OPTION

	sp1kAdcInit();
}
/**
 * @fn        void usbInit(void)
 * @brief     usbInit
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-15
 * @todo      N/A
 * @bug       N/A
*/
static void
usbInit(
	void
)
{
	G_ucPktStorIntrf = K_StorageInterface1; //current packet belongs to which storage interface
	G_ucCtrlPhase    = K_CommandPhase;
	G_ucBulkPhase    = K_CommandPhase;
	G_USBAudioSetInterface = 0; //CX add

#if (USBMSDC_OPTION == 2)
	G_USBMSDC_usSizeProcessed = 0;
	G_USBMSDC_ucSenseCodeIdx = 0x00;
	G_USBMSDC_ucCSWStatus = K_USBMSDC_CommandPassed;
	G_UIStatus = 0;//K_UISTATUS_USBMODE_PCCAM; //for prjmode!for video pv size 6/8
#endif

#if 0
	gUsbMode = K_UIOPTION_STORAGE_SIDC;
#else
    gUsbMode = K_UIOPTION_STORAGE_MSDC;
#endif

	G_MLUN_CardSupport = K_CARD_TYPE;

	XBYTE[REG_Usb_TransactionEn] &= 0xf9;			//reject (NAK)
									//next EP0 Out packet
									//next EP0 In packet

	XBYTE[REG_Usb_EpAckIntSts] = 0xff;			//Clear all status
	XBYTE[REG_Usb_IntSts] = 0xff;			//clear status
	XBYTE[REG_Usb_EpAckIntEn] = 0x07;			//enable interrupt for
									//EP0 Setup packet OK
									//EP0 Out packet OK
									//EP0 In packet OK
	XBYTE[REG_Usb_IntEn] |= 0x01;			//USBRstVTEn
#if 1
	XBYTE[REG_Usb_Status] = 0x00;			//00:Enable HS, 02:Force FS, disable SoftPlugOut !!
#else
	XBYTE[REG_Usb_Status] = 0x02;			//00:Enable HS, 02:Force FS, disable SoftPlugOut !!
#endif
	//XBYTE[0x2045] |= 0x01;			//enable GPIO8 Falling interrupt (USB detcter)
	//XBYTE[0x207B] &= ~0x01;			//clear GPIO8 Finter event
	XBYTE[0x2044] |= 0x08;			//enable GPIO3 Falling interrupt (USB detcter)
	XBYTE[0x207A] &= ~0x08;			//clear GPIO3 Finter event

#if(K_AUDIOINTER)
    XBYTE[REG_Usb_Alt_If1_NumMax] = 0x05;			/* Set alternating max to 5 */
    XBYTE[REG_Usb_UsbMode] = 0x01;  //0x256F
    XBYTE[REG_Usb_LogicEp1_Disable] = 0xe0;  //0x2530
#else
    XBYTE[REG_Usb_Alt_If1_NumMax] = 0x08;			/* Set alternating max to 8 */
    XBYTE[REG_Usb_UsbMode] = 0x03;
    XBYTE[REG_Usb_LogicEp1_Disable] = 0x00;
#endif

	HAL_UsbSwPlugOut();
    G_UI_USBConnect = K_UI_USB_DISCONNECT;
}

/**
 * @fn        void hostInit(void)
 * @brief     hostInit
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-16
 * @todo      N/A
 * @bug       N/A
*/
static void
hostInit(
	void
)
{
	UINT8 cnt = 0;
	pFuncInit* pf = &funcInitArray[0];

	/* check the array size */
	//ASSERT(sizeof(funcInitArray) / sizeof(pFuncInit) < FUNC_SYS_INIT_MAX, 0);

	while(*pf) {
		(*pf)();
		pf++;
		if (++cnt >= FUNC_SYS_INIT_MAX) {
			/* funcInitArray configure error */
			ASSERT(0, 0);
		}
	}
}

/**
 * @fn        void globalInit(void)
 * @brief     globalInit
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-15
 * @todo      N/A
 * @bug       N/A
*/
void
globalInit(
	void
)
{
	pFuncInit *pFuncInitMain = &funcInitMain;

	uartInit();                                          /* Uart initial */

	cpuInit();                                           /* 8051 initial */

	dramInit();                                          /* Dram related function initial */

	gpioInit();                                          /* GPIO initial */

	hostInit();                                          /* host initial callback */

	#if 0 //for NEW ZOOM function 20131213
	zoomDevInit();     /*Add for v2.1.8 compatibility*/
	sp1kZoomDevActSet(SP1K_ZOOM_LOGI_VIDEO_MODE,SP1K_ZOOM_LOGI_NONBLOCKING_TYPE);   //  fix mantis 45825
	#endif

	usbInit();                                           /* Usb initial */

	osInit();                                            /* system basic service initial */
	osTaskCreate(osTaskIdle, OS_PRIO_ACTIVE_FUNC);
	osMsgInit();
	cmdInit();

	sp1kDevInit();                                       /* device initial */
	sp1kAudioPlayInit();
	sp1kRTCInit();
	sp1kDiskMemInit();

	ASSERT(pFuncInitMain, 0);                            /* jump to host device initial flow */
	(*pFuncInitMain)();
	HAL_TpPreCfg();
}
