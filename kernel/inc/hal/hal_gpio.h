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
 * @file		hal_gpio.h
 * @brief		hal gpio header file.
 * @author		Matt Wang
 * @since		2008-05-27
 * @date		2009-02-25
 */
#ifndef __HAL_GPIO_H__
#define __HAL_GPIO_H__
/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
typedef enum {
	HAL_GPIO_BYTE_NULL,
	HAL_GPIO_BYTE_GEN0,   /* gpio[7:0]		*/
	HAL_GPIO_BYTE_GEN1,   /* gpio[15:8] 	*/
	HAL_GPIO_BYTE_GEN2,   /* gpio[23:16]	*/
	HAL_GPIO_BYTE_GEN3,   /* gpio[26:24]	*/
	HAL_GPIO_BYTE_FM0,	/* fmgpio[7:0]*/
	HAL_GPIO_BYTE_FM1,	/* fmgpio[15:8]*/
	HAL_GPIO_BYTE_FM2,	/* fmgpio[19:16]*/
	HAL_GPIO_BYTE_TG0,	  /* tggpio[5:0]	*/
	HAL_GPIO_BYTE_DISP0,  /* tvegpio[7:0]	*/
	HAL_GPIO_BYTE_DISP1,  /* tvegpio[15:8]	*/
	HAL_GPIO_BYTE_DISP2,  /* tvegpio[23:16] */
	HAL_GPIO_BYTE_DISP3,  /* tvegpio[27:24] */
	HAL_GPIO_BYTE_MA1,	  /* ma[14:14]		*/
	HAL_GPIO_BYTE_RGB0,   /* rgb[7:0]		*/
	HAL_GPIO_BYTE_RGB1,   /* rgb[9:8]		*/
	HAL_GPIO_BYTE_MAX,
} HAL_GpioByte_e;
 
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
void  HAL_GpioByteFuncSet(UINT8 byteId, UINT8 msk, UINT8 func);
UINT8 HAL_GpioByteFuncGet(UINT8 byteId, UINT8 msk);
void  HAL_GpioByteDirSet(UINT8 byteId, UINT8 msk, UINT8 dir);
void  HAL_GpioByteDirSet_Intr(UINT8 byteId, UINT8 msk, UINT8 dir);
UINT8 HAL_GpioByteDirGet(UINT8 byteId, UINT8 msk);
void  HAL_GpioByteOutSet(UINT8 byteId, UINT8 msk, UINT8 val);
void  HAL_GpioByteOutSet_Intr(UINT8 byteId, UINT8 msk, UINT8 val);
UINT8 HAL_GpioByteOutGet(UINT8 byteId, UINT8 msk);
void  HAL_GpioByteInGateSet(UINT8 byteId, UINT8 msk, UINT8 gate);
UINT8 HAL_GpioByteInGet(UINT8 byteId, UINT8 msk, UINT8 polarity);
void  HAL_GpioPullEnSet(UINT8 byteId, UINT8 msk, UINT8 en);
UINT8 HAL_GpioPullEnGet(UINT8 byteId, UINT8 msk);
void  HAL_GpioPullSelSet(UINT8 byteId, UINT8 msk, UINT8 sel);
UINT8 HAL_GpioPullSelGet(UINT8 byteId, UINT8 msk);
void  HAL_GpioDrivingSet(UINT8 byteId, UINT8 msk, UINT8 sel);
UINT8 HAL_GpioDrivingGet(UINT8 byteId, UINT8 msk);
UINT8 HAL_GpioSarAdcConfig(UINT8 tp_en,	UINT8 sarclk, UINT8 pwrctl);
UINT8 HAL_GpioSarAdcValGet(UINT8 channel, UINT16 *adcval);
#endif  /* __HAL_GPIO_API_H__ */

