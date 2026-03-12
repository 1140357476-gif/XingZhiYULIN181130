/**************************************************************************
 *
 *        Copyright (c) 2008-2008 by Sunplus mMedia Inc., Ltd.
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
 **************************************************************************/
/**
 * @file		gpio_api.h
 * @brief		gpio module header file.
 * @author		Matt Wang
 * @since		2008-05-28
 * @date		2009-02-25
 */
#ifndef __GPIO_API_H__
#define __GPIO_API_H__

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "hal_gpio.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
#define GPIO_USE_MACRO  1  /* fdk internal usage only. */

typedef enum {
	GPIO_BYTE_NULL,
	GPIO_BYTE_GEN0,   /* gpio[7:0]      */
	GPIO_BYTE_GEN1,   /* gpio[15:8]     */
	GPIO_BYTE_GEN2,   /* gpio[23:16]    */
	GPIO_BYTE_GEN3,   /* gpio[31:24]    */
	GPIO_BYTE_FM0,    /* fmgpio[7:0]    */
	GPIO_BYTE_FM1,    /* fmgpio[15:8]    */
	GPIO_BYTE_FM2,    /* fmgpio[19:16]  */
	GPIO_BYTE_TG0,    /* tggpio[5:0]    */
	GPIO_BYTE_DISP0,  /* tvegpio[7:0]   */
	GPIO_BYTE_DISP1,  /* tvegpio[15:8]  */
	GPIO_BYTE_DISP2,  /* tvegpio[23:16] */
	GPIO_BYTE_DISP3,  /* tvegpio[27:14] */
	GPIO_BYTE_MA1,    /* ma[14:14]      */
	GPIO_BYTE_RGB0,   /* rgb[7:0]       */
	GPIO_BYTE_RGB1,   /* rgb[9:8]       */
	GPIO_BYTE_MAX,
} gpioByte_e;  /* plz keep sync to HAL_GpioByte_e. */

/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
#if GPIO_USE_MACRO
#define gpioByteFuncSet    HAL_GpioByteFuncSet
#define gpioByteFuncGet    HAL_GpioByteFuncGet
#define gpioByteDirSet     HAL_GpioByteDirSet
#define gpioByteDirGet     HAL_GpioByteDirGet
#define gpioByteOutSet     HAL_GpioByteOutSet
#define gpioByteOutGet     HAL_GpioByteOutGet
#define gpioByteInGateSet  HAL_GpioByteInGateSet
#define gpioByteInGet      HAL_GpioByteInGet
#define gpioPullSelSet	   HAL_GpioPullSelSet
#define gpioPullSelGet	   HAL_GpioPullSelGet
#define gpioPullEnSet      HAL_GpioPullEnSet
#define gpioPullEnGet      HAL_GpioPullEnGet
#define gpioDrivingSet     HAL_GpioDrivingSet
#define gpioDrivingGet     HAL_GpioDrivingGet
#else
/* gpio_byte.c */
void  gpioByteFuncSet(UINT8 byteId, UINT8 msk, UINT8 func);
UINT8 gpioByteFuncGet(UINT8 byteId, UINT8 msk);
void  gpioByteDirSet(UINT8 byteId, UINT8 msk, UINT8 dir);
UINT8 gpioByteDirGet(UINT8 byteId, UINT8 msk);
void  gpioByteOutSet(UINT8 byteId, UINT8 msk, UINT8 val);
UINT8 gpioByteOutGet(UINT8 byteId, UINT8 msk);
void  gpioByteInGateSet(UINT8 byteId, UINT8 msk, UINT8 gate);
UINT8 gpioByteInGet(UINT8 byteId, UINT8 msk, UINT8 polarity);
void  gpioPullSelSet(UINT8 byteId, UINT8 msk, UINT8 sel);
UINT8 gpioPullSelGet(UINT8 byteId, UINT8 msk);
void  gpioPullEnSet(UINT8 byteId, UINT8 msk, UINT8 en);
UINT8 gpioPullEnGet(UINT8 byteId, UINT8 msk);
UINT8 gpioDrivingSet(UINT8 byteId, UINT8 msk, UINT8 sel);
UINT8 gpioDrivingGet(UINT8 byteId, UINT8 msk);

#endif

#endif  /* __GPIO_API_H__ */

