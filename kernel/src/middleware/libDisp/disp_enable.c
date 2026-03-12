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
 *  Author: Ted Chiu
 *
 **************************************************************************/

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "disp_api.h"
#include "reg_def.h"

/*************************************************************************
 *     Function Name: dispEn
 *     Purposes: Enable the display funciton
 *     Descriptions:
 *     Arguments:
 *    	 dispEn 0: Disable the display function
 *    	        1: Enable  the display function
 *************************************************************************/
void
dispEn(
	UINT8 en
)
{
	if (en) {
		XBYTE[REG_Disp_DispEn_Flip] |= 0x01;
		XBYTE[0x2013] |= 0x10;	//[4]:display nx clock, display 1x clock
		XBYTE[0x2015] |= 0x08;	//[3]:display mclk
		if((XBYTE[REG_Disp_VsVdIntSts] & 0x0F) < 0x02)	{	/* turn off DAC power */
			XBYTE[0x2001] &= ~0x03;  /* enable DACPd & DACTest. */
		}
		XBYTE[REG_Dram_LensGainUpdt] |= 0x04; //enable Mpgsb flow control for display 
	}
	else {
		XBYTE[REG_Disp_DispEn_Flip] &= ~0x01;
		XBYTE[0x2013] &= ~0x10;
		XBYTE[0x2015] &= ~0x08;
		if((XBYTE[REG_Disp_VsVdIntSts] & 0x0F) < 0x02)	{	/* turn off DAC power */
			XBYTE[0x2001] |=  0x03;  /* disable DACPd & DACTest. */
		}
		XBYTE[REG_Dram_LensGainUpdt] &= ~0x04; //disable Mpgsb flow control for display 
	}
	//HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PV);; // Update registers
}

/*************************************************************************
 *     Function Name: dispPvEn
 *     Purposes: enable the preview layer
 *     Descriptions:
 *     Arguments:
 *    	 dispEn 0: Disable the preview layer
 *    	           1: Enable  the preview layer
 *************************************************************************/
void
dispImgWinEn(
	UINT8 en
)
{
	if (en) {
		XBYTE[REG_Disp_DispEn_Flip] |= 0x02;
	}
	else {	//show BG only
		XBYTE[REG_Disp_DispEn_Flip] &= ~0x02;
	}

	//mask by phil@08.05.12
	XBYTE[REG_Disp_UpdPvOsd] = 0x01;
}

