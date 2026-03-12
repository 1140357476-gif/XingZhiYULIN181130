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
 * @file      gpio_byte.c
 * @brief     gpio access in byte related function.
 * @author    Matt Wang
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "hal_gpio.h"
#include "gpio_api.h" 
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
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
#if !GPIO_USE_MACRO
/**
 * @fn        void gpioByteFuncSet(UINT8 byteId, UINT8 msk, UINT8 func)
 * @brief     set gpio function in byte.
 * @param     [in] byteId: byte id, one of gpioByte_e.
 * @param     [in] msk: bitmap mask.
 * @param     [in] func: bitmap function, 0: function, 1: gpio.
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
void
gpioByteFuncSet(
	UINT8 byteId,
	UINT8 msk,
	UINT8 func
)
{
	HAL_GpioByteFuncSet(byteId, msk, func);
}


/**
 * @fn        UINT8 gpioByteFuncGet(UINT8 byteId, UINT8 msk)
 * @brief     get gpio function in byte.
 * @param     [in] byteId: byte id, one of gpioByte_e.
 * @param     [in] msk: bitmap mask.
 * @retval    bitmap function value, 0: function, 1: gpio.
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
UINT8
gpioByteFuncGet(
	UINT8 byteId,
	UINT8 msk
)
{
	return HAL_GpioByteFuncGet(byteId, msk);
}

/**
 * @fn        void gpioByteDirSet(UINT8 byteId, UINT8 msk, UINT8 dir)
 * @brief     set gpio direction in byte.
 * @param     [in] byteId: byte id, one of gpioByte_e.
 * @param     [in] msk: bitmap mask.
 * @param     [in] dir: bitmap direction, 0: input, 1: output.
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
void
gpioByteDirSet(
	UINT8 byteId,
	UINT8 msk,
	UINT8 dir
)
{
	HAL_GpioByteDirSet(byteId, msk, dir);
}

/**
 * @fn        UINT8 gpioByteDirGet(UINT8 byteId, UINT8 msk)
 * @brief     get gpio direction in byte.
 * @param     [in] byteId: byte id, one of gpioByte_e.
 * @param     [in] msk: bitmap mask.
 * @retval    bitmap direction value.
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
UINT8
gpioByteDirGet(
	UINT8 byteId,
	UINT8 msk
)
{
	return HAL_GpioByteDirGet(byteId, msk);
}

/**
 * @fn        void gpioByteOutSet(UINT8 byteId, UINT8 msk, UINT8 val)
 * @brief     set gpio output in byte.
 * @param     [in] byteId: byte id, one of gpioByte_e.
 * @param     [in] msk: bitmap mask.
 * @param     [in] val: bitmap output value.
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
void
gpioByteOutSet(
	UINT8 byteId,
	UINT8 msk,
	UINT8 val
)
{
	HAL_GpioByteOutSet(byteId, msk, val);
}

/**
 * @fn        UINT8 gpioByteOutGet(UINT8 byteId, UINT8 msk)
 * @brief     get gpio output in byte.
 * @param     [in] byteId: byte id, one of gpioByte_e.
 * @param     [in] msk: bitmap mask.
 * @retval    bitmap output value.
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
UINT8
gpioByteOutGet(
	UINT8 byteId,
	UINT8 msk
)
{
	return HAL_GpioByteOutGet(byteId, msk);
}


/**
 * @fn        void gpioByteInGateSet(UINT8 byteId, UINT8 msk, UINT8 gate)
 * @brief     set gpio input gating in byte.
 * @param     [in] byteId: byte id, one of gpioByte_e.
 * @param     [in] msk: bitmap mask.
 * @param     [in] gate: bitmap gating, 0: disable, 1: enable.
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
void
gpioByteInGateSet(
	UINT8 byteId,
	UINT8 msk,
	UINT8 gate
)
{
	HAL_GpioByteInGateSet(byteId, msk, gate);
}


/**
 * @fn        UINT8 gpioByteInGet(UINT8 byteId, UINT8 msk, UINT8 polarity)
 * @brief     get gpio input in byte.
 * @param     [in] byteId: byte id, one of gpioByte_e.
 * @param     [in] msk: bitmap mask.
 * @param     [in] polarity: hardware input 0: low active 1: high active
 * @retval    bitmap input value.
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
UINT8
gpioByteInGet(
	UINT8 byteId,
	UINT8 msk,
	UINT8 polarity
)
{
	return HAL_GpioByteInGet(byteId, msk, polarity);
}

/**
 * @fn        void gpioPullEnSet(UINT8 byteId, UINT8 msk, UINT8 en)
 * @brief     set pull high/low enable in byte.
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
 *                     		   bit [7] = ma[3].
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
 * @see       gpioPullEnSet
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
void
gpioPullEnSet(
	UINT8 byteId,
	UINT8 msk,
	UINT8 en
)
{
	HAL_GpioPullEnSet(byteId, msk, en);
}


/**
 * @fn        UINT8 gpioPullEnGet(UINT8 byteId, UINT8 msk)
 * @brief     get pull high/low enable in byte.
 * @param     [in] byteId: byte id, 0~12.
 * @param     [in] msk: bitmap mask.
 * @retval    bitmap pull high/low, 0: disable, 1: enable.
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
 * @see       gpioPullEnGet
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
UINT8
gpioPullEnGet(
	UINT8 byteId,
	UINT8 msk
)
{
	return HAL_GpioPullEnGet(byteId, msk);
}

/**
 * @fn        UINT8 gpioPullSelSet(UINT8 byteId, UINT8 msk, UINT8 sel)
 * @brief     set pull up or down
 * @param     [in] byteId: byte id, 0~9.
 * @param     [in] msk
 * @param     [in] sel: bitmap select pull up/down 0:pull down; 1:pull up
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
 * @retval    return NULL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
void 
gpioPullSelSet(
	UINT8 byteId,
	UINT8 msk,
	UINT8 sel
)
{
	return HAL_GpioPullSelSet(byteId, msk, sel);
}

/**
 * @fn        UINT8 gpioPullSelGet(UINT8 byteId, UINT8 msk)
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
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
gpioPullSelGet(
	UINT8 byteId,
	UINT8 msk
)
{
	return HAL_GpioPullSelGet(byteId, msk);
}

/**
 * @fn        UINT8 gpioDrivingSet(UINT8 byteId, UINT8 msk, UINT8 sel)
 * @brief     set gpio current 
 * @param     [in] byteId: byte id, 0~27.
 * @param     [in] msk
 * @param     [in] sel: 0:2mA
 *						1:4mA
 *						2:6mA
 *						3:8mA
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
UINT8
gpioDrivingSet(
	UINT8 byteId,
	UINT8 msk,
	UINT8 sel
)
{
	return HAL_GpioDrivingSet(byteId, msk, sel);
}

/**
 * @fn        UINT8 gpioDrivingGet(UINT8 byteId, UINT8 msk)
 * @brief     Get gpio current 
 * @param     [in] byteId
 * @param     [in] msk
 * @retval    bitmap current :0:2mA
 *						1:4mA
 *						2:6mA
 *						3:8mA
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
UINT8
gpioDrivingGet(
	UINT8 byteId,
	UINT8 msk
)
{
	return HAL_GpioDrivingGet(byteId, msk);
}
#endif

