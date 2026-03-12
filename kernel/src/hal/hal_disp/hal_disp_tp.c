/**************************************************************************
 *		   Copyright(c) 2010 by i-Catch Technology Co., Ltd.			  *
 *																		  *
 *	This software is copyrighted by and is the property of i-Catch Tech-  *
 *	nology Co., Ltd. All rights are reserved by i-Catch Technology Co.,   *
 *	Ltd. This software may only be used in accordance with the			  *
 *	corresponding license agreement. Any unauthorized use, duplication,   *
 *	distribution, or disclosure of this software is expressly forbidden.  *
 *																		  *
 *	This Copyright notice "M U S T" not be removed or modified without	  *
 *	prior written consent of i-Catch Technology Co., Ltd.				  *
 *																		  *
 *	i-Catch Technology Co., Ltd. reserves the right to modify this		  *
 *	software without notice.											  *
 *																		  *
 *	i-Catch Technology Co., Ltd.										  *
 *	19-1, Innovation First Road, Science-Based Industrial Park, 		  *
 *	Hsin-Chu, Taiwan, R.O.C.											  *
 *																		  *
 *************************************************************************/

 /**
 * @file	  touchpanel.c
 * @brief	  test for touchpanel
 * @author	  xiaolin.zhu
 * @since	  2011-07-15
 * @date
*/
/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "hal_disp_tp.h"
#include "Gpio_api.h"
#include "key_scan.h"
#include "gpio.h"

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
extern UINT16  sp1kADCValueSnap(void);
extern void sp1kADCSarSel(UINT8 sar);
extern UINT8 HAL_GpioSarAdcValGet(UINT8 channel,UINT16 * adcval);
extern UINT8 HAL_GlobalTimerWait(UINT32 WaitTime) ;
extern UINT8 HAL_GlobalTimerWait10us(UINT8 WaitTime);


/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

UINT8 HAL_TpPreCfg(void)
{
	//first,set  xn  output  1,after enable  the interrupts  we  used ,disable  xn output
	//just for avoiding  the  float voltage  bring the wrong  intrrupt
	if(pkgChipIdGet()== 0x1626)
	{
		XBYTE[0x20a4] &= 0x7f;// [ma14:gpio6] xn:gpio6   disable   xn  pull
		XBYTE[0x20a5] &= 0xfb;// [g7:g14] yn:gpio9   disable   yn  pull
		XBYTE[0x20a7] &= 0xf3;//xp yp  [23:digt3]  disable  xp  yp   pull

		XBYTE[0x2058] |= 0xc3;//  xn yn io  driving set   16ma
		XBYTE[0x205c] |= 0xc0;//xp io driving set 16 ma
		XBYTE[0x205d] |= 0x03;//yp io driving set 16 ma

		XBYTE[0x2034] |= 0x40;//enable  xn   output
		XBYTE[0x2035] |= 0x02;//enable  yn  output
		XBYTE[0x2037] &= 0xf9;//disable xp and yp  output

		XBYTE[0x2030] |= 0x40;//xn  output  1
		XBYTE[0x2031] &= ~0x02;//yn  output  0
	}
	else
	{
		XBYTE[0x20a5] &= 0x9f;// xn yn [7:14]   disable   xn  yn  pull
		XBYTE[0x20a7] &= 0xf3;//xp yp  [23:digt3]  disable  xp  yp   pull
		XBYTE[0x2059] |= 0xf0;//  xn yn io  driving set   16ma
		XBYTE[0x205c] |= 0xc0;//xp io driving set 16 ma
		XBYTE[0x205d] |= 0x03;//yp io driving set 16 ma

		XBYTE[0x2035] |= 0x10;//enable  xn   output
		XBYTE[0x2035] |= 0x20;//enable  yn  output
		XBYTE[0x2037] &= 0xf9;//disable xp and yp  output

		XBYTE[0x2031] |= 0x10;//xn  output  1
		XBYTE[0x2031] &= 0xdf;//yn  output  0
	}
	/*XBYTE[0x20a5] &= 0x9f;// xn yn [7:14]   disable   xn  yn  pull
	XBYTE[0x20a7] &= 0xf3;//xp yp  [23:digt3]  disable  xp  yp   pull
	XBYTE[0x2059] |= 0xf0;//  xn yn io  driving set   16ma
	XBYTE[0x205c] |= 0xc0;//xp io driving set 16 ma
	XBYTE[0x205d] |= 0x03;//yp io driving set 16 ma

	XBYTE[0x2035] |= 0x10;//enable  xn   output
	XBYTE[0x2035] |= 0x20;//enable  yn  output
	XBYTE[0x2037] &= 0xf9;//disable xp and yp  output

	XBYTE[0x2031] |= 0x10;//xn  output  1
	XBYTE[0x2031] &= 0xdf;//yn  output  0*/

	XBYTE[0x260b] &= 0xe7;//disable pendow interrupt  and  penup interrupt
	XBYTE[0x2603] |= 0x01;//write 1 to clear  tp_intn_evt  interrupt state
	XBYTE[0x2648] |= 0x02;//open tp_sw
	XBYTE[0x260b] |= 0x01;//enable  interrupt of tp_intn_evt

	//XBYTE[0x2035] &= 0xef;//disable  xn   output
	if(pkgChipIdGet()== 0x1626)
	{
		XBYTE[0x2034] &= ~0x40;//disable  xn   output
		XBYTE[0x2035] &= ~0x02;//disable  yn  output
	}
	else
	{
		XBYTE[0x2035] &= 0xef;//disable  xn   output
	}

	return SUCCESS;
}

void HAL_TpIntrEvt(void)
{
	XBYTE[0x260b] &= 0xfe;//disble tp evt intr
	XBYTE[0x2603] |= 0x08;//write 1to clear pendnint
	XBYTE[0x260b] |= 0x08;//enable pendnint
}

void HAL_TpYadcCfg(void)
{
	/*XBYTE[0x2035] &= 0xef;//disable  xn   output
	XBYTE[0x2035] |= 0x20;//enable  yn  output
	XBYTE[0x2037] &= 0xfd;//disable  xp  output
	XBYTE[0x2037] |= 0x04;//enable  yp  output
	XBYTE[0x2033] |= 0x04;//YP output 1
	XBYTE[0x2031] &= 0xdf;//yn output 0*/
	if(pkgChipIdGet()== 0x1626)
	{
		XBYTE[0x2034] &= ~0x40;//disable  xn   output
		XBYTE[0x2035] |= 0x02;//enable  yn  output
		XBYTE[0x2037] &= 0xfd;//disable  xp  output
		XBYTE[0x2037] |= 0x04;//enable  yp  output
		XBYTE[0x2033] |= 0x04;//YP output 1
		XBYTE[0x2031] &= ~0x02;//yn output 0
	}
	else
	{
		XBYTE[0x2035] &= 0xef;//disable  xn   output
		XBYTE[0x2035] |= 0x20;//enable  yn  output
		XBYTE[0x2037] &= 0xfd;//disable  xp  output
		XBYTE[0x2037] |= 0x04;//enable  yp  output
		XBYTE[0x2033] |= 0x04;//YP output 1
		XBYTE[0x2031] &= 0xdf;//yn output 0
	}
}

void HAL_TpXadcCfg(void)
{
    /*XBYTE[0x2648] &= 0xfd;
    XBYTE[0x2035] |= 0x10;//enable  xn   output
    XBYTE[0x2035] &= 0xdf;//disable  yn  output
    XBYTE[0x2037] |= 0x02;//enable  xp  output
    XBYTE[0x2037] &= 0xfb;//disable  yp  output
    XBYTE[0x2033] |= 0x02;//XP output 1
    XBYTE[0x2031] &= 0xef;//xn output 0*/
    	if(pkgChipIdGet()== 0x1626)
	{
		XBYTE[0x2648] &= 0xfd;
		XBYTE[0x2034] |= 0x40;//enable  xn   output
		XBYTE[0x2035] &= ~0x02;//disable  yn  output
		XBYTE[0x2037] |= 0x02;//enable  xp  output
		XBYTE[0x2037] &= 0xfb;//disable  yp  output
		XBYTE[0x2033] |= 0x02;//XP output 1
		XBYTE[0x2030] &= ~0x40;//xn output 0
	}
	else
	{
		XBYTE[0x2648] &= 0xfd;
		XBYTE[0x2035] |= 0x10;//enable  xn   output
		XBYTE[0x2035] &= 0xdf;//disable  yn  output
		XBYTE[0x2037] |= 0x02;//enable  xp  output
		XBYTE[0x2037] &= 0xfb;//disable  yp  output
		XBYTE[0x2033] |= 0x02;//XP output 1
		XBYTE[0x2031] &= 0xef;//xn output 0
	}
}

void HAL_TpadcvalPrep(void)
{
	XBYTE[0x2648]&= ~0x02;
	XBYTE[0x260b]&= ~0x18;
}

void  HAL_TpYadcStart(void)
{
	XBYTE[0x2648] |= 0x01;
	XBYTE[0x264a] = 0x03;
	XBYTE[0x264e] = 0x80;
	XBYTE[0x264f] = 0x04;
	XBYTE[0x2649] = 0x03;//chose the ad chanel 3
	XBYTE[0x264c] = 0x01; /* snap adc data */
}

void  HAL_TpXadcStart(void)
{
    XBYTE[0x2648] |= 0x01;
    XBYTE[0x264a] = 0x03;
    XBYTE[0x264e] = 0x80;
    XBYTE[0x264f] = 0x04;
    XBYTE[0x2649] = 0x04;//chose the ad chanel 4
    XBYTE[0x264c] = 0x01; /* snap adc data */
}

UINT16 HAL_TpXadcvalGet(void)
{
	UINT16 xadcval=0;

	*((UINT8*)&xadcval + 1) = XBYTE[0x264c];
	*((UINT8*)&xadcval + 0) = (XBYTE[0x264d]^0x02);
	return xadcval;
}

UINT16 HAL_TpYadcvalGet(void)
{
	UINT16 yadcval=0;

	*((UINT8*)&yadcval + 1) = XBYTE[0x264c];
	*((UINT8*)&yadcval + 0) = (XBYTE[0x264d]^0x02);
	return yadcval;
}

UINT16 HAL_TpXadcvalMsure(void)
{
	HAL_TpXadcCfg();

	XBYTE[0x2603] |= 0x04;
	HAL_TpXadcStart();
	while(!((XBYTE[0x2603]&0x04)>>2));

	XBYTE[0x2603] |= 0x04;
	return HAL_TpXadcvalGet();
}

UINT16 HAL_TpYadcvalMsure(void)
{
	HAL_TpYadcCfg();

	XBYTE[0x2603]|=0x04;
	HAL_TpYadcStart();
	while(!((XBYTE[0x2603]&0x04)>>2));

	XBYTE[0x2603]|=0x04;
	return HAL_TpYadcvalGet();
}

UINT8 HAL_TpPenupChek(void)
{
	/*XBYTE[0x2035] &= 0xef;//disable  xn   output
	XBYTE[0x2035] |= 0x20;//enable  yn  output
	XBYTE[0x2037] &= 0xf9;//disable xp and yp  output
	XBYTE[0x2031] &= 0xdf;//yn putput 0*/
	if(pkgChipIdGet()== 0x1626)
	{
		XBYTE[0x2034] &= ~0x40;//disable  xn   output
		XBYTE[0x2035] |= 0x02;//enable  yn  output
		XBYTE[0x2037] &= 0xf9;//disable xp and yp  output
		XBYTE[0x2031] &= ~0x02;//yn putput 0
	}
	else
	{
		XBYTE[0x2035] &= 0xef;//disable  xn   output
		XBYTE[0x2035] |= 0x20;//enable  yn  output
		XBYTE[0x2037] &= 0xf9;//disable xp and yp  output
		XBYTE[0x2031] &= 0xdf;//yn putput 0
	}
	XBYTE[0x2648] |= 0x02;//open tp_sw
	XBYTE[0x2603] |= 0x10;//wirte 1 to clear penupint
	XBYTE[0x260b] |= 0x10;//enable penupint


	return SUCCESS;
}

UINT8 HAL_TpReCfg(void)
{
	/*XBYTE[0x2035] |= 0x10;//enable  xn   output
	XBYTE[0x2035] |= 0x20;//enable  yn  output
	XBYTE[0x2037] &= 0xf9;//disable xp and yp  output

	XBYTE[0x2031] |= 0x10;//xn  output  1
	XBYTE[0x2031] &= 0xdf;//yn  output  0*/
	if(pkgChipIdGet()== 0x1626)
	{
		XBYTE[0x2034] |= 0x40;//enable  xn   output
		XBYTE[0x2035] |= 0x02;//enable  yn  output
		XBYTE[0x2037] &= 0xf9;//disable xp and yp  output

		XBYTE[0x2030] |= 0x40;//xn  output  1
		XBYTE[0x2031] &= ~0x02;//yn  output  0
	}
	else
	{
		XBYTE[0x2035] |= 0x10;//enable  xn   output
		XBYTE[0x2035] |= 0x20;//enable  yn  output
		XBYTE[0x2037] &= 0xf9;//disable xp and yp  output

		XBYTE[0x2031] |= 0x10;//xn  output  1
		XBYTE[0x2031] &= 0xdf;//yn  output  0
	}

	XBYTE[0x260b] &= 0xe7;//disable pendow interrupt  and  penup interrupt
	XBYTE[0x2603] |= 0x01;//write 1 to clear  tp_intn_evt  interrupt state
	XBYTE[0x2648] |= 0x02;//open tp_sw
	XBYTE[0x260b] |= 0x01;//enable  interrupt of tp_intn_evt

	//XBYTE[0x2035] &= 0xef;//disable  xn   output
	if(pkgChipIdGet()== 0x1626)
	{
		XBYTE[0x2034] &= ~0x40;//disable  xn   output
	}
	else
	{
		XBYTE[0x2035] &= 0xef;//disable  xn   output
	}

	return SUCCESS;
}

