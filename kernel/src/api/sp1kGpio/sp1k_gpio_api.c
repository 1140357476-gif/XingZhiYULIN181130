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
 * @file      sp1k_gpio_api.c
 * @brief     sp1k API for gpio configure
 * @author    sun yong
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "gpio_api.h"
#include "gpio.h"
#include "pv_photoframe.h"
#include "hal_gpio.h"
#include "sp1k_gpio_api.h"
#include "reg_def.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define USE_PKG_DEDICATE_PIN  1  /* 0: old code, 1: new code. */

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static UINT8 TV_DET_GPIO = 10;
static UINT8 PWR_KEY_GPIO = 12;
static UINT8 Debug_Mode_1527=0;

//static UINT8 G_SDCD_Polarity;
#if USE_PKG_DEDICATE_PIN
static UINT8 gpioByteIdPwrHold, gpioBitMskPwrHold, gpioPolarityPwrHold;
static UINT8 gpioByteIdStTrig, gpioBitMskStTrig, gpioPolarityStTrig;
static UINT8 gpioByteIdStChgEn, gpioBitMskStChgEn, gpioPolarityStChgEn;
static UINT8 gpioByteIdStChgRdy, gpioBitMskStChgRdy, gpioPolarityStChgRdy;
static UINT8 gpioByteIdSdCd, gpioBitMskSdCd, gpioPolaritySdCd;
static UINT8 gpioByteIdSdWp, gpioBitMskSdWp, gpioPolaritySdWp;
//static UINT8 gpioByteIdSpkEn, gpioBitMskSpkEn;
static UINT8 gpioByteIdUsbPlug, gpioBitMskUsbPlug, gpioPolarityUsbPlug;
static UINT8 gpioByteIdLcdSen, gpioBitMskLcdSen, gpioPolarityLcdSen;
static UINT8 gpioByteIdTvPlug, gpioBitMskTvPlug,  gpioPolarityTvplug;
static UINT8 gpioByteIdHDMIPlug, gpioBitMskHDMIPlug, gpioPolarityHDMIPlug;
static UINT8 gpioByteIdLedSelf, gpioBitMskLedSelf, gpioPolarityLedSelf;
static UINT8 gpioByteIdLedBusy, gpioBitMskLedBusy, gpioPolarityLedBusy;
static UINT8 gpioByteIdFrex, gpioBitMskFrex, gpioPolarityFrex;
static UINT8 gpioByteIdExpStb, gpioBitMskExpStb, gpioPolarityExpStb;
static UINT8 gpioByteIdMshEn, gpioBitMskMshEn, gpioPolarityMshEn;
static UINT8 gpioByteIdMshDir, gpioBitMskMshDir, gpioPolarityMshDir;
#endif
#if 0
static sp1kGpioPolarity_t S_GpioPolarityGrp[6] ={
{PKG_DEDICATE_PIN_SD_CD,1},
{PKG_DEDICATE_PIN_ST_CHG_RDY,1},
{PKG_DEDICATE_PIN_HDMI_PLUG,1},
{PKG_DEDICATE_PIN_TV_PLUG,1},
{PKG_DEDICATE_PIN_USB_PLUG,1},
{PKG_DEDICATE_PIN_SD_WP,1},
};
#endif
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern xdata UINT8 G_Detect_PinGrp;
extern xdata UINT8 G_Detect_PinMux;
extern xdata UINT8 G_DgpioValue;
extern UINT8 HAL_GlobalICVerGet(void);
extern UINT8 gLcdCloseFlag;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
 #if 0
/**
 * @fn		  void sp1kGpioPolaritySet(UINT8 TypeId,, UINT8 polarity)
 * @brief	  set gpio polarity.
 * @param	  [in] TypeId: pkgPin id, one of pkgDedicateGpioPin_e as following.
 *						PKG_DEDICATE_PIN_SD_CD
 *						PKG_DEDICATE_PIN_ST_CHG_RDY
 *						PKG_DEDICATE_PIN_HDMI_PLUG
 *						PKG_DEDICATE_PIN_TV_PLUG
 *						PKG_DEDICATE_PIN_USB_PLUG
 *						PKG_DEDICATE_PIN_SD_WP
 * @param	  [in] polarity: 1: high active
 *						0: low active
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.Wang
 * @since	  2010-5-17
 * @todo	  N/A
 * @bug 	  N/A
*/
void sp1kGpioPolaritySet(
	UINT8 TypeId,
	UINT8 polarity
)
{
#if USE_PKG_DEDICATE_PIN
	if (TypeId > PKG_DEDICATE_PIN_TOT) {
		return;
	}
	switch (TypeId) {
		case PKG_DEDICATE_PIN_SD_CD :
			S_GpioPolarityGrp[0].TypeId = PKG_DEDICATE_PIN_SD_CD;
			S_GpioPolarityGrp[0].polarity = polarity;
			break;
		case PKG_DEDICATE_PIN_ST_CHG_RDY:
			S_GpioPolarityGrp[1].TypeId = PKG_DEDICATE_PIN_ST_CHG_RDY;
			S_GpioPolarityGrp[1].polarity = polarity;
			break;
		case PKG_DEDICATE_PIN_HDMI_PLUG:
			S_GpioPolarityGrp[2].TypeId = PKG_DEDICATE_PIN_HDMI_PLUG;
			S_GpioPolarityGrp[2].polarity = polarity;
			break;
		case PKG_DEDICATE_PIN_TV_PLUG:
			S_GpioPolarityGrp[3].TypeId = PKG_DEDICATE_PIN_TV_PLUG;
			S_GpioPolarityGrp[3].polarity = polarity;
			break;
		case PKG_DEDICATE_PIN_USB_PLUG:
			S_GpioPolarityGrp[4].TypeId = PKG_DEDICATE_PIN_USB_PLUG;
			S_GpioPolarityGrp[4].polarity = polarity;
			break;
		case PKG_DEDICATE_PIN_SD_WP:
			S_GpioPolarityGrp[5].TypeId = PKG_DEDICATE_PIN_SD_WP;
			S_GpioPolarityGrp[5].polarity = polarity;
			break;
		default:
			break;
	}
#endif
}
#endif
/**
 * @fn        void sp1kGpioByteFuncSet(UINT8 byteId, UINT8 msk, UINT8 func)
 * @brief     set gpio function in byte.
 * @param     [in] byteId: byte id, one of sp1kGpioByte_e.
 * @param     [in] msk: bitmap mask.
 * @param     [in] func: bitmap function, 0: function, 1: gpio.
 * @retval    NONE
 * @see       NULL
 * @author    Matt Wang
 * @since     2010-5-17
 * @todo      N/A
 * @bug       N/A
*/
void
sp1kGpioByteFuncSet(
	UINT8 byteId,
	UINT8 msk,
	UINT8 func
)
{
	gpioByteFuncSet(byteId, msk, func);
}


/**
 * @fn        void sp1kGpioByteDirSet(UINT8 byteId, UINT8 msk, UINT8 dir)
 * @brief     set gpio direction in byte.
 * @param     [in] byteId: byte id, one of sp1kGpioByte_e.
 * @param     [in] msk: bitmap mask.
 * @param     [in] dir: bitmap direction, 0: input, 1: output.
 * @retval    NONE
 * @see       NULL
 * @author    Matt Wang
 * @since     2010-5-17
 * @todo      N/A
 * @bug       N/A
*/
void
sp1kGpioByteDirSet(
	UINT8 byteId,
	UINT8 msk,
	UINT8 dir
)
{
	gpioByteDirSet(byteId, msk, dir);
}


/**
 * @fn        void sp1kGpioByteOutSet(UINT8 byteId, UINT8 msk, UINT8 val)
 * @brief     set gpio output in byte.
 * @param     [in] byteId: byte id, one of sp1kGpioByte_e.
 * @param     [in] msk: bitmap mask.
 * @param     [in] val: bitmap output value.
 * @retval    NONE
 * @see       NULL
 * @author    Matt Wang
 * @since     2010-5-17
 * @todo      N/A
 * @bug       N/A
*/
void
sp1kGpioByteOutSet(
	UINT8 byteId,
	UINT8 msk,
	UINT8 val
)
{
	gpioByteOutSet(byteId, msk, val);
}

/**
 * @fn        UINT8 sp1kGpioByteInGet(UINT8 byteId, UINT8 msk, UINT8 polarity)
 * @brief     get gpio input in byte.
 * @param     [in] byteId: byte id, one of sp1kGpioByte_e.
 * @param     [in] msk: bitmap mask.
 * @param     [in] polarity: 0: low active 1: high active
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-17
 * @todo      N/A
 * @bug       N/A
*/
UINT8
sp1kGpioByteInGet(
	UINT8 byteId,
	UINT8 msk,
	UINT8 polarity
)
{
	return gpioByteInGet(byteId, msk, polarity);
}


/**
 * @fn        void sp1kPullSelSet(UINT8 byteId, UINT8 msk, UINT8 sel)
 * @brief     sp1kPullSelSet
 * @param     [in] byteId: byte id, 0~9.
 * @param     [in] msk
 * @param     [in] sel: bitmap select pull up/down 0:pull down; 1:pull up
 * @retval    NONE
 *					  if byteId = 0, bit [0] = fmgpio[0],
 *                                   bit [1] = fmgpio[1],
 *                                   bit [2] = fmgpio[2],
 *                                   bit [3] = fmgpio[3],
 *                                   bit [4] = fmgpio[4],
 *                                   bit [5] = fmgpio[5],
 *                                   bit [6] = fmgpio[6],
 *                                   bit [7] = fmgpio[7].
 *					  if byteId = 1, bit [0] = fmgpio[8],
 *                                   bit [1] = fmgpio[9],
 *                                   bit [2] = fmgpio[10],
 *                                   bit [3] = fmgpio[11],
 *                                   bit [4] = fmgpio[12],
 *                                   bit [5] = fmgpio[13],
 *                                   bit [6] = fmgpio[14],
 *                                   bit [7] = fmgpio[15].
 *					  if byteId = 2, bit [0] = fmgpio[16],
 *                                   bit [1] = fmgpio[17],
 *                                   bit [2] = fmgpio[18],
 *                                   bit [3] = fmgpio[19],
 *                                   bit [4] = ma[14],
 *                                   bit [5] = gpio[0],
 *                                   bit [6] = gpio[1],
 *                                   bit [7] = gpio[2],
 *					  if byteId = 3, bit [0] = gpio[3],
 *                                   bit [1] = gpio[4],
 *                                   bit [2] = gpio[5],
 *                                   bit [3] = gpio[6].
 *                                   bit [4] = gpio[7],
 *                                   bit [5] = gpio[8],
 *                                   bit [6] = gpio[9],
 *                                   bit [7] = gpio[10],
 *					  if byteId = 4, bit [0] = gpio[11],
 *                                   bit [1] = gpio[12],
 *                                   bit [2] = gpio[13],
 *                                   bit [3] = gpio[14],
 *                                   bit [4] = gpio[15],
 *                                   bit [5] = gpio[16],
 *                                   bit [6] = gpio[17],
 *                                   bit [7] = gpio[18],
 *					  if byteId = 5, bit [0] = gpio[19],
 *                                   bit [1] = gpio[20],
 *                                   bit [2] = gpio[21],
 *                                   bit [3] = gpio[22].
 *                                   bit [4] = gpio[23],
 *                                   bit [5] = gpio[24],
 *                                   bit [6] = sar3,
 *                                   bit [7] = sar4,
 *					  if byteId = 6, bit [0] = digtv[0],
 *                                   bit [1] = digtv[1],
 *                                   bit [2] = digtv[2],
 *                                   bit [3] = digtv[3].
 *                                   bit [4] = digtv[4],
 *                                   bit [5] = digtv[5],
 *                                   bit [6] = digtv[6],
 *                                   bit [7] = digtv[7],
 *					  if byteId = 7, bit [0] = digtv[8],
 *                                   bit [1] = digtv[9],
 *                                   bit [2] = digtv[10],
 *                                   bit [3] = digtv[11],
 *                                   bit [4] = digtv[12],
 *                                   bit [5] = digtv[13],
 *                                   bit [6] = digtv[14],
 *                                   bit [7] = digtv[15],
 *					  if byteId = 8, bit [0] = digtv[16],
 *                                   bit [1] = digtv[17],
 *                                   bit [2] = digtv[10],
 *                                   bit [3] = digtv[19],
 *                                   bit [4] = digtv[20],
 *                                   bit [5] = digtv[21],
 *                                   bit [6] = digtv[22],
 *                                   bit [7] = digtv[23],
 *					  if byteId = 9, bit [0] = digtv[24],
 *                                   bit [1] = digtv[25],
 *                                   bit [2] = digtv[26],
 *                                   bit [3] = digtv[27].
 *                                   bit [4] = tggpio[0],
 *                                   bit [5] = tggpio[1],
 *                                   bit [6] = tggpio[2],
 *                                   bit [7] = tggpio[3].
 *					  Others: Not allowed.
 * @see       NULL
 * @author    kai.wang
 * @since     2010-7-2
 * @todo      N/A
 * @bug       N/A
*/
void sp1kPullSelSet(
	UINT8 byteId,
	UINT8 msk,
	UINT8 sel
)
{
	gpioPullSelSet(byteId, msk, sel);
}

/**
 * @fn        UINT8 sp1kpullSelGet(UINT8 byteId, UINT8 msk)
 * @brief     get pull up or down
 * @param     [in] byteId: byte id, 0~9.
 * @param     [in] msk
 * @retval    bitmap pull up/down, 0:pull down; 1:pull up
  *					  if byteId = 0, bit [0] = fmgpio[0],
 *                                   bit [1] = fmgpio[1],
 *                                   bit [2] = fmgpio[2],
 *                                   bit [3] = fmgpio[3],
 *                                   bit [4] = fmgpio[4],
 *                                   bit [5] = fmgpio[5],
 *                                   bit [6] = fmgpio[6],
 *                                   bit [7] = fmgpio[7].
 *					  if byteId = 1, bit [0] = fmgpio[8],
 *                                   bit [1] = fmgpio[9],
 *                                   bit [2] = fmgpio[10],
 *                                   bit [3] = fmgpio[11],
 *                                   bit [4] = fmgpio[12],
 *                                   bit [5] = fmgpio[13],
 *                                   bit [6] = fmgpio[14],
 *                                   bit [7] = fmgpio[15].
 *					  if byteId = 2, bit [0] = fmgpio[16],
 *                                   bit [1] = fmgpio[17],
 *                                   bit [2] = fmgpio[18],
 *                                   bit [3] = fmgpio[19],
 *                                   bit [4] = ma[14],
 *                                   bit [5] = gpio[0],
 *                                   bit [6] = gpio[1],
 *                                   bit [7] = gpio[2],
 *					  if byteId = 3, bit [0] = gpio[3],
 *                                   bit [1] = gpio[4],
 *                                   bit [2] = gpio[5],
 *                                   bit [3] = gpio[6].
 *                                   bit [4] = gpio[7],
 *                                   bit [5] = gpio[8],
 *                                   bit [6] = gpio[9],
 *                                   bit [7] = gpio[10],
 *					  if byteId = 4, bit [0] = gpio[11],
 *                                   bit [1] = gpio[12],
 *                                   bit [2] = gpio[13],
 *                                   bit [3] = gpio[14],
 *                                   bit [4] = gpio[15],
 *                                   bit [5] = gpio[16],
 *                                   bit [6] = gpio[17],
 *                                   bit [7] = gpio[18],
 *					  if byteId = 5, bit [0] = gpio[19],
 *                                   bit [1] = gpio[20],
 *                                   bit [2] = gpio[21],
 *                                   bit [3] = gpio[22].
 *                                   bit [4] = gpio[23],
 *                                   bit [5] = gpio[24],
 *                                   bit [6] = sar3,
 *                                   bit [7] = sar4,
 *					  if byteId = 6, bit [0] = digtv[0],
 *                                   bit [1] = digtv[1],
 *                                   bit [2] = digtv[2],
 *                                   bit [3] = digtv[3].
 *                                   bit [4] = digtv[4],
 *                                   bit [5] = digtv[5],
 *                                   bit [6] = digtv[6],
 *                                   bit [7] = digtv[7],
 *					  if byteId = 7, bit [0] = digtv[8],
 *                                   bit [1] = digtv[9],
 *                                   bit [2] = digtv[10],
 *                                   bit [3] = digtv[11],
 *                                   bit [4] = digtv[12],
 *                                   bit [5] = digtv[13],
 *                                   bit [6] = digtv[14],
 *                                   bit [7] = digtv[15],
 *					  if byteId = 8, bit [0] = digtv[16],
 *                                   bit [1] = digtv[17],
 *                                   bit [2] = digtv[10],
 *                                   bit [3] = digtv[19],
 *                                   bit [4] = digtv[20],
 *                                   bit [5] = digtv[21],
 *                                   bit [6] = digtv[22],
 *                                   bit [7] = digtv[23],
 *					  if byteId = 9, bit [0] = digtv[24],
 *                                   bit [1] = digtv[25],
 *                                   bit [2] = digtv[26],
 *                                   bit [3] = digtv[27].
 *                                   bit [4] = tggpio[0],
 *                                   bit [5] = tggpio[1],
 *                                   bit [6] = tggpio[2],
 *                                   bit [7] = tggpio[3].
 *					  Others: Not allowed.
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-7-2
 * @todo      N/A
 * @bug       N/A
*/
UINT8 sp1kpullSelGet(
	UINT8 byteId,
	UINT8 msk
)
{
	return gpioPullSelGet(byteId, msk);
}
/**
 * @fn        void sp1kPullEnSet(UINT8 byteId, UINT8 msk, UINT8 en)
 * @brief     set pull high/low enable in byte
 * @param     [in] byteId: byte id, 0~12.
 * @param     [in] msk: bitmap mask.
 * @param     [in] en: bitmap pull high/low, 0: disable, 1: enable.
 *					  if byteId = 0, bit [0] = fmgpio[0],
 *                                   bit [1] = fmgpio[1],
 *                                   bit [2] = fmgpio[2],
 *                                   bit [3] = fmgpio[3],
 *                                   bit [4] = fmgpio[4],
 *                                   bit [5] = fmgpio[5],
 *                                   bit [6] = fmgpio[6],
 *                                   bit [7] = fmgpio[7].
 *					  if byteId = 1, bit [0] = fmgpio[8],
 *                                   bit [1] = fmgpio[9],
 *                                   bit [2] = fmgpio[10],
 *                                   bit [3] = fmgpio[11],
 *                                   bit [4] = fmgpio[12],
 *                                   bit [5] = fmgpio[13],
 *                                   bit [6] = fmgpio[14],
 *                                   bit [7] = fmgpio[15].
 *					  if byteId = 2, bit [0] = fmgpio[16],
 *                                   bit [1] = fmgpio[17],
 *                                   bit [2] = fmgpio[18],
 *                                   bit [3] = fmgpio[19],
 *                                   bit [4] = ma[0],
 *                                   bit [5] = ma[1],
 *                                   bit [6] = ma[2],
 *                                   bit [7] = ma[3].
 *					  if byteId = 3, bit [0] = ma[4],
 *                                   bit [1] = ma[5],
 *                                   bit [2] = ma[6],
 *                                   bit [3] = ma[7],
 *                                   bit [4] = ma[8],
 *                                   bit [5] = ma[9],
 *                                   bit [6] = ma[10],
 *                                   bit [7] = ma[11].
 *					  if byteId = 4, bit [0] = ma[14],
 *                                   bit [1] = gpio[0],
 *                                   bit [2] = gpio[1],
 *                                   bit [3] = gpio[2],
 *                                   bit [4] = gpio[3],
 *                                   bit [5] = gpio[4],
 *                                   bit [6] = gpio[5],
 *                                   bit [7] = gpio[6].
 *					  if byteId = 5, bit [0] = gpio[7],
 *                                   bit [1] = gpio[8],
 *                                   bit [2] = gpio[9],
 *                                   bit [3] = gpio[10],
 *                                   bit [4] = gpio[11],
 *                                   bit [5] = gpio[12],
 *                                   bit [6] = gpio[13],
 *                                   bit [7] = gpio[14].
 *					  if byteId = 6, bit [0] = gpio[15],
 *                                   bit [1] = gpio[16],
 *                                   bit [2] = gpio[17],
 *                                   bit [3] = gpio[18],
 *                                   bit [4] = gpio[19],
 *                                   bit [5] = gpio[20],
 *                                   bit [6] = gpio[21],
 *                                   bit [7] = gpio[22].
 *					  if byteId = 7, bit [0] = gpio[23],
 *                                   bit [1] = gpio[24],
 *                                   bit [2] = sar3,
 *                                   bit [3] = sar4,
 *                                   bit [4] = digtv[0],
 *                                   bit [5] = digtv[1],
 *                                   bit [6] = digtv[2],
 *                                   bit [7] = digtv[3].
 *					  if byteId = 8, bit [0] = digtv[4],
 *                                   bit [1] = digtv[5],
 *                                   bit [2] = digtv[6],
 *                                   bit [3] = digtv[7],
 *                                   bit [4] = digtv[8],
 *                                   bit [5] = digtv[9],
 *                                   bit [6] = digtv[10],
 *                                   bit [7] = digtv[11],
 *					  if byteId = 9, bit [0] = digtv[12],
 *                                   bit [1] = digtv[13],
 *                                   bit [2] = digtv[14],
 *                                   bit [3] = digtv[15],
 *                                   bit [4] = digtv[16],
 *                                   bit [5] = digtv[17],
 *                                   bit [6] = digtv[10],
 *                                   bit [7] = digtv[19],
 *					  if byteId = 10, bit [0] = digtv[20],
 *                                   bit [1] = digtv[21],
 *                                   bit [2] = digtv[22],
 *                                   bit [3] = digtv[23],
 *                                   bit [4] = digtv[24],
 *                                   bit [5] = digtv[25],
 *                                   bit [6] = digtv[26],
 *                                   bit [7] = digtv[27].
 *					  if byteId = 11, bit [0] = rgb[0],
 *                                   bit [1] = rgb[1],
 *                                   bit [2] = rgb[2],
 *                                   bit [3] = rgb[3],
 *                                   bit [4] = rgb[4],
 *                                   bit [5] = rgb[5],
 *                                   bit [6] = rgb[6],
 *                                   bit [7] = rgb[7].
 *					  if byteId = 12, bit [0] = rgb[8],
 *                                   bit [1] = rgb[9],
 *                                   bit [2] = tggpio[0],
 *                                   bit [3] = tggpio[1],
 *                                   bit [4] = tggpio[2],
 *                                   bit [5] = tggpio[3],
 *                                   bit [6] = tggpio[4],
 *                                   bit [7] = tggpio[5].
 *					  Others: Not allowed.
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-17
 * @todo      N/A
 * @bug       N/A
*/
void
sp1kPullEnSet(
	UINT8 byteId,
	UINT8 msk,
	UINT8 en
)
{
	gpioPullEnSet(byteId, msk, en);
}


/**
 * @fn        UINT8 sp1kPullEnGet(UINT8 byteId, UINT8 msk)
 * @brief     get pull high/low enable in byte.
 * @param     [in] byteId: byte id, 0~9.
 * @param     [in] msk: bitmap mask.
 * @retval    bitmap pull up/down, 0:pull down; 1:pull up
 *					  if byteId = 0, bit [0] = fmgpio[0],
 *                                   bit [1] = fmgpio[1],
 *                                   bit [2] = fmgpio[2],
 *                                   bit [3] = fmgpio[3],
 *                                   bit [4] = fmgpio[4],
 *                                   bit [5] = fmgpio[5],
 *                                   bit [6] = fmgpio[6],
 *                                   bit [7] = fmgpio[7].
 *					  if byteId = 1, bit [0] = fmgpio[8],
 *                                   bit [1] = fmgpio[9],
 *                                   bit [2] = fmgpio[10],
 *                                   bit [3] = fmgpio[11],
 *                                   bit [4] = fmgpio[12],
 *                                   bit [5] = fmgpio[13],
 *                                   bit [6] = fmgpio[14],
 *                                   bit [7] = fmgpio[15].
 *					  if byteId = 2, bit [0] = fmgpio[16],
 *                                   bit [1] = fmgpio[17],
 *                                   bit [2] = fmgpio[18],
 *                                   bit [3] = fmgpio[19],
 *                                   bit [4] = ma[14],
 *                                   bit [5] = gpio[0],
 *                                   bit [6] = gpio[1],
 *                                   bit [7] = gpio[2],
 *					  if byteId = 3, bit [0] = gpio[3],
 *                                   bit [1] = gpio[4],
 *                                   bit [2] = gpio[5],
 *                                   bit [3] = gpio[6].
 *                                   bit [4] = gpio[7],
 *                                   bit [5] = gpio[8],
 *                                   bit [6] = gpio[9],
 *                                   bit [7] = gpio[10],
 *					  if byteId = 4, bit [0] = gpio[11],
 *                                   bit [1] = gpio[12],
 *                                   bit [2] = gpio[13],
 *                                   bit [3] = gpio[14],
 *                                   bit [4] = gpio[15],
 *                                   bit [5] = gpio[16],
 *                                   bit [6] = gpio[17],
 *                                   bit [7] = gpio[18],
 *					  if byteId = 5, bit [0] = gpio[19],
 *                                   bit [1] = gpio[20],
 *                                   bit [2] = gpio[21],
 *                                   bit [3] = gpio[22].
 *                                   bit [4] = gpio[23],
 *                                   bit [5] = gpio[24],
 *                                   bit [6] = sar3,
 *                                   bit [7] = sar4,
 *					  if byteId = 6, bit [0] = digtv[0],
 *                                   bit [1] = digtv[1],
 *                                   bit [2] = digtv[2],
 *                                   bit [3] = digtv[3].
 *                                   bit [4] = digtv[4],
 *                                   bit [5] = digtv[5],
 *                                   bit [6] = digtv[6],
 *                                   bit [7] = digtv[7],
 *					  if byteId = 7, bit [0] = digtv[8],
 *                                   bit [1] = digtv[9],
 *                                   bit [2] = digtv[10],
 *                                   bit [3] = digtv[11],
 *                                   bit [4] = digtv[12],
 *                                   bit [5] = digtv[13],
 *                                   bit [6] = digtv[14],
 *                                   bit [7] = digtv[15],
 *					  if byteId = 8, bit [0] = digtv[16],
 *                                   bit [1] = digtv[17],
 *                                   bit [2] = digtv[10],
 *                                   bit [3] = digtv[19],
 *                                   bit [4] = digtv[20],
 *                                   bit [5] = digtv[21],
 *                                   bit [6] = digtv[22],
 *                                   bit [7] = digtv[23],
 *					  if byteId = 9, bit [0] = digtv[24],
 *                                   bit [1] = digtv[25],
 *                                   bit [2] = digtv[26],
 *                                   bit [3] = digtv[27].
 *                                   bit [4] = tggpio[0],
 *                                   bit [5] = tggpio[1],
 *                                   bit [6] = tggpio[2],
 *                                   bit [7] = tggpio[3].
 *					  Others: Not allowed.
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-17
 * @todo      N/A
 * @bug       N/A
*/
UINT8
sp1kPullEnGet(
	UINT8 byteId,
	UINT8 msk
)
{
	return gpioPullEnGet(byteId, msk);
}


//-----------------------------------------------------------------------------
//sp1kSD_CdCfg
//-----------------------------------------------------------------------------
/**
 * @fn        void sp1kSD_CdCfg(void)
 * @brief     SD card  detect  configure
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-18
 * @todo      N/A
 * @bug       N/A
*/
void sp1kSD_CdCfg(void)
{
	//fmgpio4
	gpioDrivingSet(1, 0x01, 3);
	//fmgpio5
	gpioDrivingSet(1, 0x02, 3);
	//fmgpio6
	gpioDrivingSet(1, 0x04, 3);
	//fmgpio7
	gpioDrivingSet(1, 0x08, 3);
	//fmgpio8
	gpioDrivingSet(2, 0x01, 3);
	//fmgpio9
	gpioDrivingSet(2, 0x02, 3);
	
	#if USE_PKG_DEDICATE_PIN
	if((pkgChipIdGet() == 0x1627)||(pkgChipIdGet() == 0x1626))
	{
		G_Detect_PinGrp = 0xff;//gpioByteIdSdCd;
		G_Detect_PinMux = 0xff;//gpioBitMskSdCd;
		if(HAL_GlobalICVerGet() == 0x01)
		{
			XBYTE[REG_Disp_Rsv0] |= 0x04;//dgpio use as card detect pin, enable HW DGPIO detect
		}
		else
		{
			XBYTE[REG_Disp_VsVdIntEn] |= 0x02;//dgpio use as card detect pin, enable INTR DGPIO detect
		}
	}
	else
	{
		gpioByteFuncSet(GPIO_BYTE_GEN0, (1 << (1 & 0x07)), (1 << (1 & 0x07)));
		gpioByteDirSet(GPIO_BYTE_GEN0, (1 << (1 & 0x07)), (1 << (0 & 0x07)));
		pkgDedicateGpioCfg(PKG_DEDICATE_PIN_SD_CD, &gpioByteIdSdCd, &gpioBitMskSdCd, &gpioPolaritySdCd,0);
		// gpioPullEnSet(4, 0x04, 0x04);  // gpio[1] pull high
		
		G_Detect_PinGrp = gpioByteIdSdCd;
		G_Detect_PinMux = gpioBitMskSdCd;
	}
	#endif
}

//-----------------------------------------------------------------------------
//sp1kSD_CdGet
//-----------------------------------------------------------------------------
/**
 * @fn        UINT8 sp1kSD_CdGet(void)
 * @brief     SD card  detect value get
 * @param     NONE
 * @retval    current detect value
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-18
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable
UINT8 sp1kSD_CdGet(void)
{
	#if USE_PKG_DEDICATE_PIN
	UINT8 val=0;
	
	if((pkgChipIdGet() == 0x1627) || pkgChipIdGet()== 0x1626)
	{
		if(HAL_GlobalICVerGet() == 0x01)
		{
			val = XBYTE[REG_Disp_Rsv3]&0x01;
		}
		else
		{
			val = G_DgpioValue&0x01;
		}
	}
	else
	{
		val = HAL_GpioByteInGet(gpioByteIdSdCd, gpioBitMskSdCd, gpioPolaritySdCd);
	}
	
	return  val;
	#endif
}

//#ifndef _DBG_CLR_WARNING_
//-----------------------------------------------------------------------------
//sp1kST_TrigCfg
//-----------------------------------------------------------------------------
/**
 * @fn        void sp1kSTChgEnCfg(void)
 * @brief     ST Charge Enable configure
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-18
 * @todo      N/A
 * @bug       N/A
*/
void sp1kSTChgEnCfg(void)
{
	#if USE_PKG_DEDICATE_PIN
	pkgDedicateGpioCfg(PKG_DEDICATE_PIN_ST_CHG_EN, &gpioByteIdStChgEn, &gpioBitMskStChgEn, &gpioPolarityStChgEn,1);

	#endif
}

//-----------------------------------------------------------------------------
//sp1kST_TrigSet
//-----------------------------------------------------------------------------

/**
 * @fn        void sp1kSTChgEnSet(UINT8 value)
 * @brief     ST Charge Enable set
 * @param     [in] value: set value
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-18
 * @todo      N/A
 * @bug       N/A
*/
void sp1kSTChgEnSet(UINT8 value)
{
	#if USE_PKG_DEDICATE_PIN
	gpioByteOutSet(gpioByteIdStChgEn, gpioBitMskStChgEn, value?0xFF:0x00);

	#endif
}


/**
 * @fn        void sp1kST_TrigCfg(void)
 * @brief     ST trig configure
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-18
 * @todo      N/A
 * @bug       N/A
*/
void sp1kST_TrigCfg(void)
{
	#if USE_PKG_DEDICATE_PIN
	pkgDedicateGpioCfg(PKG_DEDICATE_PIN_ST_TRIG, &gpioByteIdStTrig, &gpioBitMskStTrig, &gpioPolarityStTrig, 1);

	#endif
}

//-----------------------------------------------------------------------------
//sp1kST_TrigSet
//-----------------------------------------------------------------------------
/**
 * @fn        void sp1kST_TrigSet(UINT8 value)
 * @brief     ST trig Set
 * @param     [in] value: set value
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-18
 * @todo      N/A
 * @bug       N/A
*/
void sp1kST_TrigSet(UINT8 value)
{
	#if USE_PKG_DEDICATE_PIN
	gpioByteOutSet(gpioByteIdStTrig, gpioBitMskStTrig, value?0xFF:0x00);

	#endif
}

//-----------------------------------------------------------------------------
//sp1kSTChgRdyCfg
//-----------------------------------------------------------------------------
/**
 * @brief		ST CHG ready configure
 * @param	NONE
 * @retval	NONE
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kSTChgRdyCfg(void)
{
	#if USE_PKG_DEDICATE_PIN
	pkgDedicateGpioCfg(PKG_DEDICATE_PIN_ST_CHG_RDY, &gpioByteIdStChgRdy, &gpioBitMskStChgRdy, &gpioPolarityStChgRdy,0);

	#endif
}

//-----------------------------------------------------------------------------
//sp1kSTChgRdyGet
//-----------------------------------------------------------------------------
/**
 * @brief		ST CHG ready value get
 * @param	NONE
 * @retval	value: current ST CHG ready Get
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
UINT8 sp1kSTChgRdyGet(void)
{
	#if USE_PKG_DEDICATE_PIN
		return gpioByteInGet(gpioByteIdStChgRdy, gpioBitMskStChgRdy, gpioPolarityStChgRdy);
	#endif
}

//-----------------------------------------------------------------------------
//sp1kFrexCfg																		  //Jim@0903 - for M_SHT
//-----------------------------------------------------------------------------
/**
 * @brief		Frex configure
 * @param	NONE
 * @retval	NONE
 * @see
 * @author	Jima
 * @since		2008-09-03
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kFrexCfg(void)		//Jim@0903 - for M_SHT
{
	#if USE_PKG_DEDICATE_PIN
	pkgDedicateGpioCfg(PKG_DEDICATE_PIN_FREX, &gpioByteIdFrex, &gpioBitMskFrex, &gpioPolarityFrex, 1);

	#endif
}

//-----------------------------------------------------------------------------
//sp1kFrexSet
//-----------------------------------------------------------------------------
/**
 * @brief		Frex Set
 * @param	value : set value
 * @retval	NONE
 * @see
 * @author	Jima
 * @since		2008-09-03
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kFrexSet(UINT8 value)		//Jim@0903 - for M_SHT
{
	#if USE_PKG_DEDICATE_PIN
	gpioByteOutSet(gpioByteIdFrex, gpioBitMskFrex, value?0xFF:0x00);

	#endif
}

void
sp1kFrexSet_Intr(
	UINT8 *value
)
{
	#if USE_PKG_DEDICATE_PIN
	gpioByteOutSet(gpioByteIdFrex, gpioBitMskFrex, *value?0xFF:0x00);
	#endif
}

//-----------------------------------------------------------------------------
//sp1kExpstbCfg
//-----------------------------------------------------------------------------
/**
 * @brief		Expstb configure
 * @param	NONE
 * @retval	NONE
 * @see
 * @author	Jima
 * @since		2008-09-03
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kExpstbCfg(void)		//Jim@0903 - for M_SHT
{
	#if USE_PKG_DEDICATE_PIN
	pkgDedicateGpioCfg(PKG_DEDICATE_PIN_EXP_STB, &gpioByteIdExpStb, &gpioBitMskExpStb, &gpioPolarityExpStb, 1);

	#endif
}

//-----------------------------------------------------------------------------
//sp1kExpstbSet
//-----------------------------------------------------------------------------
/**
 * @brief		Expstb Set
 * @param	value : set value
 * @retval	NONE
 * @see
 * @author	Jima
 * @since		2008-09-03
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kExpstbSet(UINT8 value)		//Jim@0903 - for M_SHT
{
	#if USE_PKG_DEDICATE_PIN
	gpioByteOutSet(gpioByteIdExpStb, gpioBitMskExpStb, value?0xFF:0x00);

	#endif
}

//-----------------------------------------------------------------------------
//sp1kSHT_ACfg
//-----------------------------------------------------------------------------
/**
 * @brief		SHT_A configure
 * @param	NONE
 * @retval	NONE
 * @see
 * @author	Jima
 * @since		2008-09-03
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kSHT_ACfg(void)		//Jim@0903 - for M_SHT
{
	#if USE_PKG_DEDICATE_PIN
	pkgDedicateGpioCfg(PKG_DEDICATE_PIN_MSH_EN, &gpioByteIdMshEn, &gpioBitMskMshEn, &gpioPolarityMshEn, 1);

	#endif
}

//-----------------------------------------------------------------------------
//sp1kSHT_ASet
//-----------------------------------------------------------------------------
/**
 * @brief		SHT_A Set
 * @param	value : set value
 * @retval	NONE
 * @see
 * @author	Jima
 * @since		2008-09-03
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kSHT_ASet(UINT8 value)		//Jim@0903 - for M_SHT
{
	#if USE_PKG_DEDICATE_PIN
	gpioByteOutSet(gpioByteIdMshEn, gpioBitMskMshEn, value?0xFF:0x00);

	#endif
}

//-----------------------------------------------------------------------------
//sp1kSHT_BCfg
//-----------------------------------------------------------------------------
/**
 * @brief		SHT_B configure
 * @param	NONE
 * @retval	NONE
 * @see
 * @author	Jima
 * @since		2008-09-03
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kSHT_BCfg(void)		//Jim@0903 - for M_SHT
{
	#if USE_PKG_DEDICATE_PIN
	pkgDedicateGpioCfg(PKG_DEDICATE_PIN_MSH_DIR, &gpioByteIdMshDir, &gpioBitMskMshDir, &gpioPolarityMshDir, 1);

	#endif
}

//-----------------------------------------------------------------------------
//sp1kSHT_BSet
//-----------------------------------------------------------------------------
/**
 * @brief		SHT_B Set
 * @param	value : set value
 * @retval	NONE
 * @see
 * @author	Jima
 * @since		2008-09-03
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kSHT_BSet(UINT8 value)		//Jim@0903 - for M_SHT
{
	#if USE_PKG_DEDICATE_PIN
	gpioByteOutSet(gpioByteIdMshDir, gpioBitMskMshDir, value?0xFF:0x00);

	#endif
}

//#endif /*_DBG_CLR_WARNING_*/

//-----------------------------------------------------------------------------
//sp1kPWR_HoldCfg
//-----------------------------------------------------------------------------
/**
 * @brief		power hold gpio configure
 * @param	NONE
 * @retval	NONE
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kPWR_HoldCfg(void)
{
	#if USE_PKG_DEDICATE_PIN
	pkgDedicateGpioCfg(PKG_DEDICATE_PIN_PWR_HOLD, &gpioByteIdPwrHold, &gpioBitMskPwrHold, &gpioPolarityPwrHold, 1);

	#endif
}

//-----------------------------------------------------------------------------
//sp1kPWR_HoldSet
//-----------------------------------------------------------------------------
/**
 * @brief		power hold gpio Set
 * @param	value : set value
 * @retval	NONE
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kPWR_HoldSet(UINT8 value)
{
	#if USE_PKG_DEDICATE_PIN
	gpioByteOutSet(gpioByteIdPwrHold, gpioBitMskPwrHold, value?0xFF:0x00);

	#endif
}

#ifndef _DBG_CLR_WARNING_
//-----------------------------------------------------------------------------
//sp1kPWR_KeyCfg
//-----------------------------------------------------------------------------
/**
 * @brief		power key gpio configure
 * @param	NONE
 * @retval	NONE
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kPWR_KeyCfg(UINT8 gpio)
{
	PWR_KEY_GPIO = gpio;
	HAL_ConfigGPIOBit(PWR_KEY_GPIO, input);

	return;
}

//-----------------------------------------------------------------------------
//sp1kPWR_KeyGet
//-----------------------------------------------------------------------------
/**
 * @brief		power key value get
 * @param	NONE
 * @retval	value : power key value get
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
UINT8 sp1kPWR_KeyGet(void)
{
	UINT8 value;

	HAL_GetGPIOBit(PWR_KEY_GPIO, &value);

	return value;
}
#endif /*_DBG_CLR_WARNING_*/

//-----------------------------------------------------------------------------
//sp1kSpeaker_ENCfg
//-----------------------------------------------------------------------------
/**
 * @brief		speeker enable configure
 * @param	NONE
 * @retval	NONE
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.

void sp1kSpeaker_ENCfg(void)
{
	#if USE_PKG_DEDICATE_PIN
	pkgDedicateGpioCfg(PKG_DEDICATE_PIN_SPK_EN, &gpioByteIdSpkEn, &gpioBitMskSpkEn, 1);

	#endif
}
*/

//-----------------------------------------------------------------------------
//sp1kSpeaker_ENGet
//-----------------------------------------------------------------------------
/**
 * @brief		speeker enable get
 * @param	NONE
 * @retval	value : speeker enable get
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
//UINT8 sp1kSpeaker_ENSet(UINT8 value)
//{
/*	#if USE_PKG_DEDICATE_PIN
	#if TAG_TODO
	if(appPanelTypeGet()) {
		gpioByteOutSet(gpioByteIdSpkEn, gpioBitMskSpkEn, value?0xFF:0x00);
	}
	else
	#endif
	{
		gpioByteOutSet(gpioByteIdSpkEn, gpioBitMskSpkEn, value?0xFF:0x00);
	}

	#endif
*/
//	return SUCCESS;
//}


//-----------------------------------------------------------------------------
//sp1kLED_SelfCfg
//-----------------------------------------------------------------------------
/**
 * @brief		self snap LED configure
 * @param	NONE
 * @retval	NONE
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kLED_SelfCfg(void)
{
	#if USE_PKG_DEDICATE_PIN
	pkgDedicateGpioCfg(PKG_DEDICATE_PIN_LED_SELF, &gpioByteIdLedSelf, &gpioBitMskLedSelf, &gpioPolarityLedSelf, 1);
	return;
	#endif
}

//-----------------------------------------------------------------------------
//sp1kLED_SelfSet
//-----------------------------------------------------------------------------
/**
 * @brief		self snap LED Set
 * @param	value : set value
 * @retval	NONE
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kLED_SelfSet(UINT8 value)
{
	#if USE_PKG_DEDICATE_PIN
	gpioByteOutSet(gpioByteIdLedSelf, gpioBitMskLedSelf, value?0xFF:0x00);

	#endif
}

//-----------------------------------------------------------------------------
//sp1kLED_BusyCfg
//-----------------------------------------------------------------------------
/**
 * @brief		camera busy led configure
 * @param	NONE
 * @retval	NONE
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kLED_BusyCfg(void)
{
	#if USE_PKG_DEDICATE_PIN
	pkgDedicateGpioCfg(PKG_DEDICATE_PIN_LED_BUSY, &gpioByteIdLedBusy, &gpioBitMskLedBusy, &gpioPolarityLedBusy, 1);
	#endif
}

//-----------------------------------------------------------------------------
//sp1kLED_BusySet
//-----------------------------------------------------------------------------
/**
 * @brief		camera busy led set
 * @param	value : set value
 * @retval	NONE
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kLED_BusySet(UINT8 value)
{
	#if USE_PKG_DEDICATE_PIN
	gpioByteOutSet(gpioByteIdLedBusy, gpioBitMskLedBusy, value?0xFF:0x00);
	#endif
}

//#ifndef _DBG_CLR_WARNING_
//-----------------------------------------------------------------------------
//sp1kLCD_SenCfg
//-----------------------------------------------------------------------------
/**
 * @brief		LCD SEN configure
 * @param	NONE
 * @retval	NONE
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kLCD_SenCfg(void)
{
	#if USE_PKG_DEDICATE_PIN
	pkgDedicateGpioCfg(PKG_DEDICATE_PIN_LCD_SEN, &gpioByteIdLcdSen, &gpioBitMskLcdSen, &gpioPolarityLcdSen, 1);
	#endif
}

//-----------------------------------------------------------------------------
//sp1kLCD_SenSet
//-----------------------------------------------------------------------------
/**
 * @brief		LCD SEN Set
 * @param	value : set value
 * @retval	NONE
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kLCD_SenSet(UINT8 value)
{
	#if USE_PKG_DEDICATE_PIN
	gpioByteOutSet(gpioByteIdLcdSen, gpioBitMskLcdSen, value?0xFF:0x00);
	#endif
}
//#endif /*_DBG_CLR_WARNING_*/


//-----------------------------------------------------------------------------
//sp1kHDMI_PlugCfg
//-----------------------------------------------------------------------------
/**
 * @brief		sp1k Turnkey HDMI plug detect configure
 * @param	NONE
 * @retval	NONE
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kHDMI_PlugCfg(void)
{
	#if USE_PKG_DEDICATE_PIN
	if(pkgChipIdGet()== 0x1626)
	{
		//gpioPullSelSet(3,0x40,0x00);
		//gpioPullEnSet(5,0x04,0x00);//GPIO9 pull dis
	}
	else
	{
		//XBYTE[0x20a7] &= ~0x02; //gpio 24 pull low disable
	}
	pkgDedicateGpioCfg(PKG_DEDICATE_PIN_HDMI_PLUG, &gpioByteIdHDMIPlug, &gpioBitMskHDMIPlug, &gpioPolarityHDMIPlug, 0);

	#endif
}

//-----------------------------------------------------------------------------
//sp1kHDMI_PlugGet
//-----------------------------------------------------------------------------
/**
 * @brief		sp1k Turnkey HDMI plug detect value get
 * @param	NONE
 * @retval	value: HDMI plug detect value
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
UINT8 sp1kHDMI_PlugGet(void)
{
	#if USE_PKG_DEDICATE_PIN
		return gpioByteInGet(gpioByteIdHDMIPlug, gpioBitMskHDMIPlug, gpioPolarityHDMIPlug);
	#endif
}
//-----------------------------------------------------------------------------
//sp1kTV_PlugCfg
//-----------------------------------------------------------------------------
/**
 * @brief		sp1k Turnkey TV plug detect configure
 * @param	NONE
 * @retval	NONE
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kTV_PlugCfg(UINT8 gpio, UINT8 value)
{
	#if USE_PKG_DEDICATE_PIN

	gpio = gpio;
	value = value;

	pkgDedicateGpioCfg(PKG_DEDICATE_PIN_TV_PLUG, &gpioByteIdTvPlug, &gpioBitMskTvPlug, &gpioPolarityTvplug, 0);
	#endif
}

//-----------------------------------------------------------------------------
//sp1kTV_PlugGet
//-----------------------------------------------------------------------------
/**
 * @brief		sp1k Turnkey TV plug detect value get
 * @param	NONE
 * @retval	value: TV plug detect value
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
UINT8 sp1kTV_PlugGet(void)
{
	#if USE_PKG_DEDICATE_PIN
		if(pkgChipIdGet()== 0x1626) {
			return 0x20;
		} else {
			return 0x04; // gpioByteInGet(gpioByteIdTvPlug, gpioBitMskTvPlug, gpioPolarityTvplug);
		}
	#endif
}

//-----------------------------------------------------------------------------
//sp1kUSB_PlugCfg
//-----------------------------------------------------------------------------
/**
 * @brief		sp1k Turnkey USB plug detect configure
 * @param	NONE
 * @retval	NONE
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kUSB_PlugCfg(void)
{

	#if USE_PKG_DEDICATE_PIN
	//sp1kPullSelSet(3, 0x01, 0x00);
	sp1kPullEnSet(4, 0x10, 0);
	pkgDedicateGpioCfg(PKG_DEDICATE_PIN_USB_PLUG, &gpioByteIdUsbPlug, &gpioBitMskUsbPlug, &gpioPolarityUsbPlug, 0);
	#endif
}

//-----------------------------------------------------------------------------
//sp1kUSB_PlugGet
//-----------------------------------------------------------------------------
/**
 * @brief		sp1k Turnkey USB plug detect value get
 * @param	NONE
 * @retval	value: USB plug detect value
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
UINT8 sp1kUSB_PlugGet(void)
{
	UINT8 val = 0xff;
	
	#if USE_PKG_DEDICATE_PIN
	val = gpioByteInGet(gpioByteIdUsbPlug, gpioBitMskUsbPlug, gpioPolarityUsbPlug);
	return val;
	#endif
}

#ifndef _DBG_CLR_WARNING_
//-----------------------------------------------------------------------------
//sp1kShutterKeyCfg
//-----------------------------------------------------------------------------
/**
 * @brief		sp1k shutter key configure
 * @param	NONE
 * @retval	NONE
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kShutterKeyCfg(void)
{

	HAL_GlobalWriteRegister(SHUTTERKEY_CFG, HAL_GlobalReadRegister(SHUTTERKEY_CFG) | 0x10)
	return;
}

//-----------------------------------------------------------------------------
//sp1kShutterKeySet
//-----------------------------------------------------------------------------
/**
 * @brief		sp1k shutter key Set
 * @param	value : set value
 * @retval	NONE
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kShutterKeySet(UINT8 value)
{
	HAL_GlobalWriteRegister(SHUTTERKEY_VLU, value);

	return;
}
#endif /*_DBG_CLR_WARNING_*/

//-----------------------------------------------------------------------------
//sp1kSDWPDectCfg
//-----------------------------------------------------------------------------
/**
 * @brief		SD card write protect detect configure
 * @param	NONE
 * @retval	NONE
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kSDWPDectCfg(void)
{
	#if USE_PKG_DEDICATE_PIN
	if((pkgChipIdGet() == 0x1627)||(pkgChipIdGet() == 0x1626))
	{
		if(HAL_GlobalICVerGet() == 0x01)
		{
			XBYTE[REG_Disp_Rsv0] |= 0x04;//dgpio use as card detect pin, enable HW DGPIO detect
		}
		else
		{
			XBYTE[REG_Disp_VsVdIntEn] |= 0x02;//dgpio use as card detect pin, enable INTR DGPIO detect
		}
	}
	else
	{
		pkgDedicateGpioCfg(PKG_DEDICATE_PIN_SD_WP, &gpioByteIdSdWp, &gpioBitMskSdWp, &gpioPolaritySdWp, 0);
	}
	#endif
}

//-----------------------------------------------------------------------------
//sp1kSDWriteProtectDetectGet
//-----------------------------------------------------------------------------
/**
 * @brief		SD card write protect detect value get
 * @param	NONE
 * @retval	current detect value
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
UINT8 sp1kSDWriteProtectDetectGet(void)
{
	UINT8 val = 0x00;
	
	#if USE_PKG_DEDICATE_PIN
	
	return 0;
	#endif
}
/**
 * @brief	Enable/Disable TG
 * @param	Enable
 * @retval	None
 * @see
 * @author	Alle Lin
 * @since	2008-12-11
 * @todo	N/A
 * @bug		N/A
*/
void sp1kHalTgEnable(
	UINT8 Enable
)
{
	if (Enable == 0){
		/* disable TG */
		HAL_GpioByteFuncSet(GPIO_BYTE_TG0, 0x0C, 0xFF);
	}
	else{
		/* enable TG */
		HAL_GpioByteFuncSet(GPIO_BYTE_TG0, 0x0C, 0x00);
	}
}

void sp1kADCSarSel(UINT8 sar)
{
	XBYTE[REG_Audio_SarChnlSel] = sar;
}

UINT8 sp1kADCSarGet(void)
{
	return XBYTE[REG_Audio_SarChnlSel];
}

UINT16  sp1kADCValueSnap(void)
{
	UINT16 adcVal;
	XBYTE[0x2603] |= 0x04;
	XBYTE[REG_Audio_SnapSarAdcVal0] = 0x01;
	while(!((XBYTE[0x2603]&0x04)>>2));
	XBYTE[0x2603] |= 0x04;
	*((UINT8*)&adcVal + 1) = XBYTE[REG_Audio_SnapSarAdcVal0];
	*((UINT8*)&adcVal + 0) = (XBYTE[REG_Audio_SnapSarAdcVal1]^0x02);
	return adcVal;
}

UINT16  sp1kADCValueSnapIntr(void)
{
	UINT16 adcVal;
	XBYTE[0x2603] |= 0x04;
	XBYTE[REG_Audio_SnapSarAdcVal0] = 0x01;
	while(!((XBYTE[0x2603]&0x04)>>2));
	XBYTE[0x2603] |= 0x04;
	*((UINT8*)&adcVal + 1) = XBYTE[REG_Audio_SnapSarAdcVal0];
	*((UINT8*)&adcVal + 0) = (XBYTE[REG_Audio_SnapSarAdcVal1]^0x02);
	return adcVal;
}


//-----------------------------------------------------------------------------
//sp1kAdcInit
//-----------------------------------------------------------------------------
/**
 * @brief		initialize adc gpio
 * @param	NONE
 * @retval	NONE
 * @see
 * @author	phil
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kAdcInit(void)
{
	HAL_GpioSarAdcConfig(0,3,1);
	sp1kADCSarSel(0);
}

/**
 * @fn        UINT8 sp1kAdcValGet(UINT8 channel, UINT16 *adcval)
 * @brief     sp1kAdcValGet
 * @param     [in] channel
 * @param     [in] adcval
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-18
 * @todo      N/A
 * @bug       N/A
*/
UINT8 sp1kAdcValGet(UINT8 channel, UINT16 * adcval)

{
	return HAL_GpioSarAdcValGet(channel, adcval);
}
