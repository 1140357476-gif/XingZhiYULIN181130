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
 * @file		package_1628.c
 * @brief		chip package 1628 driver.
 * @author		Matt Wang
 * @since		2010-01-01
 * @date		2010-01-01
 */

/*
 * History
 *
 * 1000, 20081117 Matt Wang
 *    a. Created
 */

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "gpio.h"
#include "gpio_api.h"
#include "sys_mem_init.h"
#include "solution.h"
/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
 #if(_HW_SOLUTION_ == _HW_RDK_A_)
code pkgDedicateGpio_t pkgDedicateGpioTbl[] = {
	{PKG_DEDICATE_PIN_PWR_HOLD,   GPIO_BYTE_GEN0,  (1 << ( 7 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_S2,         GPIO_BYTE_GEN1, (1 << (9 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_ST_TRIG,    GPIO_BYTE_GEN2,  (1 << ( 16 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_ST_CHG_EN,  GPIO_BYTE_GEN1,  (1 << ( 14 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_ST_CHG_RDY, GPIO_BYTE_GEN1,  (1 << ( 15 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_SD_CD,      GPIO_BYTE_GEN0,  (1 << (1 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_SD_WP,      GPIO_BYTE_GEN0,   (1 << (0 & 0x07)), ACTIVE_HIGH},
	//{PKG_DEDICATE_PIN_SPK_EN,     GPIO_BYTE_NULL,  NULL             },
	{PKG_DEDICATE_PIN_PWMDACA,    GPIO_BYTE_NULL,  NULL , ACTIVE_HIGH            },
	{PKG_DEDICATE_PIN_USB_PLUG,   GPIO_BYTE_GEN1,  (1 << ( 8 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_LCD_SEN,    GPIO_BYTE_GEN1,  (1 << ( 9 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_TV_PLUG,    GPIO_BYTE_GEN0,  (1 << (2 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_HDMI_PLUG,  GPIO_BYTE_GEN3,  (1 << (24 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_LED_SELF,   GPIO_BYTE_GEN0,  (1 << (4 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_LED_BUSY,   GPIO_BYTE_GEN0,  (1 << (5 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_FREX,       GPIO_BYTE_GEN1,  (1 << (14 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_EXP_STB,    GPIO_BYTE_GEN1,  (1 << (15 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_MSH_EN,     GPIO_BYTE_GEN2,  (1 << (16 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_MSH_DIR,    GPIO_BYTE_GEN2,  (1 << (17 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_MLC_SDPWRCTL, GPIO_BYTE_NULL,  NULL, ACTIVE_HIGH              },
	{PKG_DEDICATE_PIN_MLC_NANDEN, 	 GPIO_BYTE_NULL,  NULL, ACTIVE_HIGH              },//GPIO_BYTE_FM0,   (1 << (1 & 0x07))},
	{PKG_DEDICATE_PIN_MLC_SDEN,       GPIO_BYTE_NULL,  NULL, ACTIVE_HIGH              },//GPIO_BYTE_FM0,   (1 << (3 & 0x07))},

};
#elif(_HW_SOLUTION_ == _HW_CUSTOM_MODEL_A_)
code pkgDedicateGpio_t pkgDedicateGpioTbl[] = {
	{PKG_DEDICATE_PIN_PWR_HOLD,   GPIO_BYTE_GEN0,  (1 << ( 7 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_S2,         GPIO_BYTE_GEN1,  (1 << (12 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_ST_TRIG,    GPIO_BYTE_GEN2,  (1 << ( 16 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_ST_CHG_EN,  GPIO_BYTE_GEN1,  (1 << ( 14 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_ST_CHG_RDY, GPIO_BYTE_GEN1,  (1 << ( 15 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_SD_CD,      GPIO_BYTE_GEN0,  (1 << (1 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_SD_WP,      GPIO_BYTE_GEN0,   (1 << (0 & 0x07)), ACTIVE_HIGH},
	//{PKG_DEDICATE_PIN_SPK_EN,     GPIO_BYTE_NULL,  NULL             },
	{PKG_DEDICATE_PIN_PWMDACA,    GPIO_BYTE_NULL,  NULL, ACTIVE_HIGH             },
	{PKG_DEDICATE_PIN_USB_PLUG,   GPIO_BYTE_GEN1,  (1 << ( 8 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_LCD_SEN,    GPIO_BYTE_GEN1,  (1 << ( 9 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_TV_PLUG,    GPIO_BYTE_GEN0,  (1 << (2 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_HDMI_PLUG,  GPIO_BYTE_GEN3,  (1 << (24 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_LED_SELF,   GPIO_BYTE_GEN0,  (1 << (4 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_LED_BUSY,   GPIO_BYTE_GEN0,  (1 << (5 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_FREX,       GPIO_BYTE_GEN1,  (1 << (14 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_EXP_STB,    GPIO_BYTE_GEN1,  (1 << (15 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_MSH_EN,     GPIO_BYTE_GEN2,  (1 << (16 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_MSH_DIR,    GPIO_BYTE_NULL,  NULL, ACTIVE_HIGH  			 },
	{PKG_DEDICATE_PIN_MLC_SDPWRCTL, GPIO_BYTE_NULL,  NULL, ACTIVE_HIGH              },
	{PKG_DEDICATE_PIN_MLC_NANDEN, 	 GPIO_BYTE_NULL,  NULL, ACTIVE_HIGH             },//GPIO_BYTE_FM0,   (1 << (1 & 0x07))},
	{PKG_DEDICATE_PIN_MLC_SDEN,       GPIO_BYTE_NULL,  NULL, ACTIVE_HIGH              },//GPIO_BYTE_FM0,   (1 << (3 & 0x07))},

};
#elif(_HW_SOLUTION_ == _HW_CUSTOM_MODEL_B_)
code pkgDedicateGpio_t pkgDedicateGpioTbl[] = {
	{PKG_DEDICATE_PIN_PWR_HOLD,   GPIO_BYTE_GEN0,  (1 << ( 7 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_S2,         GPIO_BYTE_GEN1,  (1 << (12 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_ST_TRIG,    GPIO_BYTE_GEN2,  (1 << ( 16 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_ST_CHG_EN,  GPIO_BYTE_GEN1,  (1 << ( 14 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_ST_CHG_RDY, GPIO_BYTE_GEN1,  (1 << ( 15 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_SD_CD,      GPIO_BYTE_GEN0,  (1 << (1 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_SD_WP,      GPIO_BYTE_GEN0,   (1 << (0 & 0x07)), ACTIVE_HIGH},
	//{PKG_DEDICATE_PIN_SPK_EN,     GPIO_BYTE_NULL,  NULL             },
	{PKG_DEDICATE_PIN_PWMDACA,    GPIO_BYTE_NULL,  NULL, ACTIVE_HIGH            },
	{PKG_DEDICATE_PIN_USB_PLUG,   GPIO_BYTE_GEN1,  (1 << ( 8 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_LCD_SEN,    GPIO_BYTE_GEN1,  (1 << ( 9 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_TV_PLUG,    GPIO_BYTE_GEN0,  (1 << (2 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_HDMI_PLUG,  GPIO_BYTE_GEN3,  (1 << (24 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_LED_SELF,   GPIO_BYTE_GEN0,  (1 << (4 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_LED_BUSY,   GPIO_BYTE_GEN0,  (1 << (5 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_FREX,       GPIO_BYTE_GEN1,  (1 << (14 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_EXP_STB,    GPIO_BYTE_GEN1,  (1 << (15 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_MSH_EN,     GPIO_BYTE_GEN2,  (1 << (16 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_MSH_DIR,    GPIO_BYTE_NULL,  NULL, ACTIVE_HIGH   },
	{PKG_DEDICATE_PIN_MLC_SDPWRCTL, GPIO_BYTE_NULL,  NULL, ACTIVE_HIGH              },
	{PKG_DEDICATE_PIN_MLC_NANDEN, 	 GPIO_BYTE_NULL,  NULL, ACTIVE_HIGH              },//GPIO_BYTE_FM0,   (1 << (1 & 0x07))},
	{PKG_DEDICATE_PIN_MLC_SDEN,       GPIO_BYTE_NULL,  NULL, ACTIVE_HIGH              },//GPIO_BYTE_FM0,   (1 << (3 & 0x07))},

};

#else

code pkgDedicateGpio_t pkgDedicateGpioTbl[] = {
	{PKG_DEDICATE_PIN_PWR_HOLD,   GPIO_BYTE_GEN0,  (1 << ( 7 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_S2,         GPIO_BYTE_GEN0, (1 << (1 & 0x07)), ACTIVE_LOW},
	{PKG_DEDICATE_PIN_ST_TRIG,     GPIO_BYTE_NULL,  NULL, ACTIVE_HIGH              },
	{PKG_DEDICATE_PIN_ST_CHG_EN,   GPIO_BYTE_NULL,  NULL, ACTIVE_HIGH              },
	{PKG_DEDICATE_PIN_ST_CHG_RDY,  GPIO_BYTE_NULL,  NULL, ACTIVE_HIGH              },
	{PKG_DEDICATE_PIN_SD_CD,      GPIO_BYTE_DISP0,  (1 << (1 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_SD_WP,       GPIO_BYTE_DISP0,  (1 << (0 & 0x07)), ACTIVE_HIGH},
	//{PKG_DEDICATE_PIN_SPK_EN,     GPIO_BYTE_NULL,  NULL                },
	{PKG_DEDICATE_PIN_PWMDACA,    GPIO_BYTE_NULL,  NULL, ACTIVE_HIGH              },
	{PKG_DEDICATE_PIN_USB_PLUG,   GPIO_BYTE_GEN1,  (1 << ( 8 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_LCD_SEN,    GPIO_BYTE_GEN0,  (1 << ( 0 & 0x07), ACTIVE_HIGH)          },
	{PKG_DEDICATE_PIN_TV_PLUG,    GPIO_BYTE_GEN0,  (1 << (5 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_HDMI_PLUG, GPIO_BYTE_GEN1,  (1 << (9 & 0x07)), ACTIVE_HIGH},
	{PKG_DEDICATE_PIN_LED_SELF,   GPIO_BYTE_NULL,  NULL, ACTIVE_HIGH			},
	{PKG_DEDICATE_PIN_LED_BUSY,   GPIO_BYTE_NULL,  NULL, ACTIVE_HIGH			},
	{PKG_DEDICATE_PIN_FREX,      GPIO_BYTE_NULL,  NULL, ACTIVE_HIGH              },
	{PKG_DEDICATE_PIN_EXP_STB,    GPIO_BYTE_NULL,  NULL, ACTIVE_HIGH              },
	{PKG_DEDICATE_PIN_MSH_EN,     GPIO_BYTE_NULL,  NULL, ACTIVE_HIGH             },
	{PKG_DEDICATE_PIN_MSH_DIR,    GPIO_BYTE_NULL,  NULL, ACTIVE_HIGH              },
	{PKG_DEDICATE_PIN_MLC_SDPWRCTL, GPIO_BYTE_NULL,  NULL, ACTIVE_HIGH              },
	{PKG_DEDICATE_PIN_MLC_NANDEN, 	 GPIO_BYTE_NULL,  NULL, ACTIVE_HIGH              },//GPIO_BYTE_FM0,   (1 << (1 & 0x07))},
	{PKG_DEDICATE_PIN_MLC_SDEN,       GPIO_BYTE_NULL,  NULL, ACTIVE_HIGH             },//GPIO_BYTE_FM0,   (1 << (3 & 0x07))},

};



#endif
code UINT8 pkgDedicateGpioTot = sizeof(pkgDedicateGpioTbl) / sizeof(pkgDedicateGpio_t);

code UINT16 pkgChipId = 0x1626;

