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
 * @file      key_scan.h
 * @brief     Brief
 * @author    Phil
 * @since     2010-01-01
 * @date      2010-01-01
*/
#ifndef __KEY_SCAN_H__
#define __KEY_SCAN_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "button_def.h"
#include "solution.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/


#define KEY_SCAN_ADC_VAL_MIN			0x0023U//0x0010U

#define KEY_SCAN_ADC_VAL_MAX			0x03ffU
#if (_HW_SOLUTION_ == _HW_EVB_ || _HW_SOLUTION_ == _HW_TK_MODEL_A_ )
#define KEY_SCAN_ADC_A_VAL_IDLE		(KEY_SCAN_ADC_VAL_MIN + 0x2FDU)//0x16D
#else
#define KEY_SCAN_ADC_A_VAL_IDLE		(KEY_SCAN_ADC_VAL_MIN + 0x2FD)
#endif


#if (_HW_SOLUTION_ == _HW_EVB_ || _HW_SOLUTION_ == _HW_TK_MODEL_A_)
#define KEY_SCAN_ADC_B_VAL_IDLE		(KEY_SCAN_ADC_VAL_MIN + 0x0FU)//0x0fU
#else
#define KEY_SCAN_ADC_B_VAL_IDLE		(KEY_SCAN_ADC_VAL_MIN + 0x0fU)
#endif

#define TP_PEN_RPT           			10  				// was 12, 0 ~ 16; 0:No repeate key; 1:More frequently
#define TP_PEN_RPT_START				40 				// define the delay time to do the first repeat key

#define KEY_SCAN_GPIO_VAL_RPT           1  				// was 12, 0 ~ 16; 0:No repeate key; 1:More frequently
#define KEY_SCAN_GPIO_VAL_RPT_START		100 				// define the delay time to do the first repeat key (5ms??бь?иж??им???им???им??????им??????)

#define KEY_SCAN_ADC_VAL_RPT			2	//Key scan: 10ms
#define KEY_SCAN_ADC_VAL_RPT_START		10//20

#define KEY_SCAN_ADC_VAL_DEC			2


/* ADC button val*/
#if (_HW_SOLUTION_ == _HW_RDK_A_)
#define SP1K_ADC_BTN_A_S1			994//1010//1000		/* RIGHT */
#define SP1K_ADC_BTN_A_S2			924//946//931		/* LEFT */
#define SP1K_ADC_BTN_A_S3			827//852//835		/* DOWN */
#define SP1K_ADC_BTN_A_S4			676//696//680		/* UP */
#define SP1K_ADC_BTN_A_S5			519//530//524		/* TELE */
#define SP1K_ADC_BTN_A_S6			329//335//332		/* WIDE */
#elif (_HW_SOLUTION_ == _HW_CUSTOM_MODEL_A_)
#define SP1K_ADC_BTN_A_S1			853		/* RIGHT */
#define SP1K_ADC_BTN_A_S2			696		/* LEFT */
#define SP1K_ADC_BTN_A_S3			536	/* DOWN */
#define SP1K_ADC_BTN_A_S4			341
#else
#define SP1K_ADC_BTN_A_S1			1023	/*POWER*/	
//#define SP1K_ADC_BTN_A_S2			810		/*DEL*/	
//#define SP1K_ADC_BTN_A_S3			1023	/*OK*/ 	
//#define SP1K_ADC_BTN_A_S4			600		/*POWER*/ 	
#endif

#if (_HW_SOLUTION_ == _HW_EVB_)
#define SP1K_ADC_BTN_B_S1			559		/* SET1 */
#define SP1K_ADC_BTN_B_S2			860		/* UP1 */
#define SP1K_ADC_BTN_B_S3			715 	/* BACK1 */
#define SP1K_ADC_BTN_B_S4         	93 		/* PLAY1 */
#define SP1K_ADC_BTN_B_S5			409 	/* DOWN1 */
#define SP1K_ADC_BTN_B_S6			1023 	/* OK1 */
#define SP1K_ADC_BTN_B_S7			275 	/* A/B1 */
#elif (_HW_SOLUTION_ == _HW_TK_MODEL_A_)
#define SP1K_ADC_BTN_B_S1         	326		/* DISP */
#define SP1K_ADC_BTN_B_S2			490		/* MODE */
#define SP1K_ADC_BTN_B_S3			685		/* OK/MENU */

#elif (_HW_SOLUTION_ == _HW_TK_MODEL_B_)
#define SP1K_ADC_BTN_B_S1			170//224		/* POWER */
#define SP1K_ADC_BTN_B_S2         	326//373		/* DISP */
#define SP1K_ADC_BTN_B_S3			490//506		/* MODE */
#define SP1K_ADC_BTN_B_S4			685//684		/* OK/MENU */

#elif (_HW_SOLUTION_ == _HW_RDK_A_)
#define SP1K_ADC_BTN_B_S1			329//340//333		/*menu */
#define SP1K_ADC_BTN_B_S2         		676//680//682		/* PB */
//#define SP1K_ADC_BTN_B_S3			597		/* F1 */
#define SP1K_ADC_BTN_B_S3			519//530//527		/* OK */
#define SP1K_ADC_BTN_B_S4			184//180//185		/* POWER */
#elif (_HW_SOLUTION_ == _HW_CUSTOM_MODEL_A_)
#define SP1K_ADC_BTN_B_S1			341	/*menu */
#define SP1K_ADC_BTN_B_S2         		696	/* mode */
#endif //hw solution


/* ADC button dec*/
#if (_HW_SOLUTION_ == _HW_RDK_A_)
#define SP1K_ADC_BTN_DEV_A_S1		30
#define SP1K_ADC_BTN_DEV_A_S2		30
#define SP1K_ADC_BTN_DEV_A_S3		50
#define SP1K_ADC_BTN_DEV_A_S4		50
#define SP1K_ADC_BTN_DEV_A_S5		80
#define SP1K_ADC_BTN_DEV_A_S6		80
#elif(_HW_SOLUTION_ == _HW_CUSTOM_MODEL_A_)
#define SP1K_ADC_BTN_DEV_A_S1		60
#define SP1K_ADC_BTN_DEV_A_S2		60
#define SP1K_ADC_BTN_DEV_A_S3		50
#define SP1K_ADC_BTN_DEV_A_S4		25
#else
#define SP1K_ADC_BTN_DEV_A_S1		100
//#define SP1K_ADC_BTN_DEV_A_S2		100
//#define SP1K_ADC_BTN_DEV_A_S3		100
//#define SP1K_ADC_BTN_DEV_A_S4		100
#endif


#if (_HW_SOLUTION_ == _HW_EVB_)
#define SP1K_ADC_BTN_DEV_B_S1		50
#define SP1K_ADC_BTN_DEV_B_S2		45
#define SP1K_ADC_BTN_DEV_B_S3		50
#define SP1K_ADC_BTN_DEV_B_S4		50
#define SP1K_ADC_BTN_DEV_B_S5		50
#define SP1K_ADC_BTN_DEV_B_S6		45
#define SP1K_ADC_BTN_DEV_B_S7		50

#elif(_HW_SOLUTION_ == _HW_TK_MODEL_A_)
#define SP1K_ADC_BTN_DEV_B_S1		52
#define SP1K_ADC_BTN_DEV_B_S2		78
#define SP1K_ADC_BTN_DEV_B_S3		95
#elif (_HW_SOLUTION_ == _HW_TK_MODEL_B_)
#define SP1K_ADC_BTN_DEV_B_S1		29
#define SP1K_ADC_BTN_DEV_B_S2		52
#define SP1K_ADC_BTN_DEV_B_S3		78
#define SP1K_ADC_BTN_DEV_B_S4		95
#elif (_HW_SOLUTION_ == _HW_RDK_A_)
#define SP1K_ADC_BTN_DEV_B_S1		50
#define SP1K_ADC_BTN_DEV_B_S2		50
#define SP1K_ADC_BTN_DEV_B_S3		50
#define SP1K_ADC_BTN_DEV_B_S4		50
//#define SP1K_ADC_BTN_DEV_B_S5		20
#elif (_HW_SOLUTION_ == _HW_CUSTOM_MODEL_A_)
#define SP1K_ADC_BTN_DEV_B_S1		100
#define SP1K_ADC_BTN_DEV_B_S2		100

#endif

/* ADC button num*/
#if (_HW_SOLUTION_ == _HW_CUSTOM_MODEL_A_)
#define SP1K_ADC_BTN_A_NUM		4
#else
#define SP1K_ADC_BTN_A_NUM		1
#endif



#if (_HW_SOLUTION_ == _HW_EVB_)
#define SP1K_ADC_BTN_B_NUM		7
#elif (_HW_SOLUTION_ == _HW_TK_MODEL_A_)
#define SP1K_ADC_BTN_B_NUM		3
#elif (_HW_SOLUTION_ == _HW_TK_MODEL_B_)
#define SP1K_ADC_BTN_B_NUM		4
#elif (_HW_SOLUTION_ == _HW_RDK_A_)
#define SP1K_ADC_BTN_B_NUM		4
#elif (_HW_SOLUTION_ == _HW_CUSTOM_MODEL_A_)
#define SP1K_ADC_BTN_B_NUM		2
#endif

#define SP1K_ADC_BTN_NUM			(SP1K_ADC_BTN_A_NUM + SP1K_ADC_BTN_B_NUM)


/*GPIO button status*/
#define SP1K_GPIO_BTN_A_STATUS	KEY_STATUS_RELEASED		/*Power*/
#define SP1K_GPIO_BTN_B_STATUS	KEY_STATUS_RELEASED		/*S2*/

/*GPIO button num*/

#if (_HW_SOLUTION_ == _HW_CUSTOM_MODEL_A_)
#define SP1K_GPIO_BTN_NUM			2
#else
#define SP1K_GPIO_BTN_NUM			4			//+1 only for 1626 evb
#endif


#define SP1K_BTN_NUM				(SP1K_ADC_BTN_NUM + SP1K_GPIO_BTN_NUM)

#define POWER_BAT_DEC		5
#define BATT_LOW_POWER_OFF_RETRY		((UINT16)100)		//100*30ms
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define UI_TEST  1
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef enum keyStatus_e {
	KEY_STATUS_PRESSED = (unsigned char)0x01,
	KEY_STATUS_RELEASED = (unsigned char)0x02,
	KEY_STATUS_REPEAT = (unsigned char)0x04,
} keyStatus_t;

typedef enum keyType_e {
	KEY_TYPE_ADC_A = (unsigned char)0x01,
	KEY_TYPE_ADC_B = (unsigned char)0x02,
	KEY_TYPE_GPIO = (unsigned char)0x03,
} keyType_t;

typedef struct keyAttrAdc_s {
	UINT8 keyNum;
	UINT8 keyType;
	UINT8* pKeyEn;
} keyAttrAdc_t;

typedef struct keyAttrGpio_s {
	UINT8 keyStsPrev;
	UINT8* pKeyEn;
} keyAttrGpio_t;

typedef struct keyMsg_s {
	UINT16 keyMsgPrs;
	UINT16 keyMsgRpt;
	UINT16 keyMsgRls;
} keyMsg_t;//

typedef struct keyCtrlAdc_s {
	UINT8 keyDec;
	UINT16 code* pKeyVal;
	UINT16 code* pKeyDev;
	UINT16 keyValPrev;
	UINT16 keyValRpt;
} keyCtrlAdc_t;

typedef struct keyCtrlGpio_s {
	UINT8 keyDec;
	UINT16 keyValRpt;
} keyCtrlGpio_t;

typedef struct keyUnitAdc_s {
	struct keyAttrAdc_s keyAttr;
	struct keyMsg_s code* pKeyMsg;
	struct keyCtrlAdc_s keyCtrl;
	UINT8 keyActiveIdx;
} keyUnitAdc_t;

typedef struct keyUnitGpio_s {
	struct keyAttrGpio_s keyAttr[SP1K_GPIO_BTN_NUM];
	struct keyMsg_s code* pKeyMsg;
	struct keyCtrlGpio_s keyCtrl;
} keyUnitGpio_t;

typedef struct battUnitAdc_s {
	UINT8 battLvl;
	UINT8 battDec;
	UINT8 battDetectEn;
	UINT8 battLvlTol;
	UINT16* pbattAdcVal;
	UINT16 battAdcDev;
} battUnitAdc_t;

typedef struct scanTag_s {
	struct keyUnitAdc_s keyAdcA;
	struct keyUnitAdc_s keyAdcB;
	struct keyUnitGpio_s keyGpio;
	struct battUnitAdc_s battAdc;
} scanTag_t;

/*snap no disable*/
#define sp1kBtnDisableAll()	do {\
								sp1kBtnCfg(KEY_TYPE_ADC_A, 0xffffU, 0x00);\ 
								sp1kBtnCfg(KEY_TYPE_ADC_B, 0xfffeU, 0x00);\
								sp1kBtnCfg(KEY_TYPE_GPIO, 0xffffU, 0x00);\
							} while(0)

#define sp1kBtnEnableAll()	do {\
								sp1kBtnCfg(KEY_TYPE_ADC_A, 0xffffU, 0xff);\
								sp1kBtnCfg(KEY_TYPE_ADC_B, 0xffffU, 0xff);\
								sp1kBtnCfg(KEY_TYPE_GPIO, 0xffffU, 0xff);\
							} while(0)


#define sp1kBtnDisableAllIntr()	do {\
								sp1kBtnCfgIntr(KEY_TYPE_ADC_A, 0xffffU, 0x00);\
								sp1kBtnCfgIntr(KEY_TYPE_ADC_B, 0xfffeU, 0x00);\
								sp1kBtnCfgIntr(KEY_TYPE_GPIO, 0xffffU, 0x00);\
							} while(0)

#define sp1kBtnEnableAllIntr()	do {\
								sp1kBtnCfgIntr(KEY_TYPE_ADC_A, 0xffffU, 0xff);\
								sp1kBtnCfgIntr(KEY_TYPE_ADC_B, 0xffffU, 0xff);\
								sp1kBtnCfgIntr(KEY_TYPE_GPIO, 0xffffU, 0xff);\
							} while(0)




/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void keyScanInit(void);
void keyScan(void);
UINT8 sp1kBtnCfg(keyType_t BtnType, UINT16 BitMask, UINT8 Enable);
UINT8 sp1kBtnCfgIntr(keyType_t BtnType, UINT16 BitMask, UINT8 Enable);
void sp1kAdcHdlGet(const scanTag_t** pAdcHdl);
UINT8 battDetectInit(void);
UINT8 battDetectCfg(UINT8 battLvlTol, UINT16* pBattAdcVal, UINT16 battAdvValDev);
void sp1kBattEnable(UINT8 enable);
UINT8 sp1kBtnChk(UINT8 button);
#endif  /* __KEY_SCAN_H__ */



