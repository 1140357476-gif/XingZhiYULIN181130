/**************************************************************************
 *
 *        Copyright (c) 2007-2008 by Sunplus mMedia Inc., Ltd.
 *
 *  This software is copyrighted by and is the property of Sunplus
 *  mMedia Inc., Ltd. All rights are reserved by Sunplus mMedia
 *  Inc., Ltd. This software may only be used in accordance with the
 *  corresponding license agreement. Any unauthorized use, duplication,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of Sunplus mMedia Inc., Ltd.
 *
 *  Sunplus mMedia Inc., Ltd. reserves the right to modify this
 *  software without notice.
 *
 *  Sunplus mMedia Inc., Ltd.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 *  Author: sunyong
 *
 **************************************************************************/
#ifndef __GPIO_H__
#define __GPIO_H__

/**
 * @filename 	GPIO.h
 * @brief 	The header file of  gpio
 * @author 	sun yong
 * @since 	2008-03-01
 * @date 		2008-03-01
 */
/**************************************************************************
 *							  H E A D E R	F I L E S
 **************************************************************************/
#include "sp1k_gpio_api.h"

/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/
//#if (!FPGA_VERIFY)
//#define sD_CdGet() 0
//#else
#define sD_CdGet		sp1kSD_CdGet
//#endif

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
#define input		0
#define output 	1
#define GENGPIO			(UINT16)0x00

#define ST_TRIG			(GENGPIO + 0)
#define PWR_ON_EN		(GENGPIO + 1)
#define ST_CHG_RDY		(GENGPIO + 2)

#define PWMDAC_A		(GENGPIO + 5)
#define LED_SELF			(GENGPIO + 7)

#define ST_CHG_EN		(GENGPIO + 8)
#define LCD_SEN			(GENGPIO + 9)

#define USB_PLUG		(GENGPIO + 6)
//#define TV_PLUG_EVB			(GENGPIO + 6)
#define TV_PLUG			(GENGPIO + 10)

#define SD_CDn			(GENGPIO + 11)
#define PWR_ON_KEY_EVB		(GENGPIO + 12)
#define SPEAKER_ON_TKY		(GENGPIO + 12)

#define LED_BUSY		(GENGPIO + 13)

#define Frex			(GENGPIO + 14)	   //Jim@0903 - for M_SHT
#define Expstb			(GENGPIO + 15)
#define SHT_A			(GENGPIO + 16)
#define SHT_B			(GENGPIO + 17)

#define MA14_CFG		(UINT16)REG_Dram_SdckMode
#define MA14_VLU		(UINT16)REG_Dram_Dummy0
#define SD_WP_CFG		MA14_CFG
#define SD_WP_VLU		MA14_VLU

#define DISPGPIO12_CFG	(UINT16)0x2D89
#define DISPGPIO12_VLU	(UINT16)0x2D85
#define SHUTTERKEY_CFG	DISPGPIO12_CFG
#define SHUTTERKEY_VLU	DISPGPIO12_VLU

typedef enum {
	PKG_DEDICATE_PIN_PWR_HOLD,
	PKG_DEDICATE_PIN_S2,
	PKG_DEDICATE_PIN_ST_TRIG,
	PKG_DEDICATE_PIN_ST_CHG_EN,
	PKG_DEDICATE_PIN_ST_CHG_RDY,
	PKG_DEDICATE_PIN_SD_CD,
	PKG_DEDICATE_PIN_SD_WP,
	//PKG_DEDICATE_PIN_SPK_EN,
	PKG_DEDICATE_PIN_PWMDACA,
	PKG_DEDICATE_PIN_USB_PLUG,
	PKG_DEDICATE_PIN_LCD_SEN,
	PKG_DEDICATE_PIN_TV_PLUG,
	PKG_DEDICATE_PIN_HDMI_PLUG,
	PKG_DEDICATE_PIN_LED_SELF,
	PKG_DEDICATE_PIN_LED_BUSY,
	PKG_DEDICATE_PIN_FREX,
	PKG_DEDICATE_PIN_EXP_STB,
	PKG_DEDICATE_PIN_MSH_EN,
	PKG_DEDICATE_PIN_MSH_DIR,
	PKG_DEDICATE_PIN_MLC_SDPWRCTL,
	PKG_DEDICATE_PIN_MLC_NANDEN,
	PKG_DEDICATE_PIN_MLC_SDEN,
	PKG_DEDICATE_PIN_TOT,
} pkgDedicateGpioPin_e;


#define NORMAL_MODE			0
#define DEBUG_USB_MODE 		1
#define DEBUG_NON_USB_MODE 	2

#define ACTIVE_LOW	0	//negative
#define ACTIVE_HIGH	1	//Positive
/**************************************************************************
 *                              D A T A   T Y P E S
 **************************************************************************/
typedef struct {
	UINT8 pinId;
	UINT8 byteId;
	UINT8 bitMsk;
	UINT8 polarity;
} pkgDedicateGpio_t;

/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
void pkgDedicateGpioCfg(UINT8 pinId, UINT8 *pbyteId, UINT8 *pbitMsk, UINT8*ppolarity, UINT8 dir);
UINT8 pkgDedicateGpioGet(UINT8 pinId, UINT8 *pbyteId, UINT8 *pbitMsk, UINT8 *ppolarity);
UINT16 pkgChipIdGet(void);
UINT16 pkgChipIdGet_Intr(void);

#endif //__GPIO_H__

