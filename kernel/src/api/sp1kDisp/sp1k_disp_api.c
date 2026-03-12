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
 * @file		sp1k_disp_api.c
 * @brief		switch between display device.
 * @author		Ted Chiu, Matt Wang
 * @since		2008-04-29
 * @date		2008-12-09
 */

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "initio.h"
#include "hal.h"
#include "hal_dram.h"
#include "hal_disp.h"
#include "hal_front.h"
#include "hal_front_para.h"
#include "disp_api.h"
#include "app_dev_disp.h"
#include "reg_def.h"
/* for mantis 44031  Fit show for D1 video in PB mode */
#include "app_still_play.h"
#include "sp1k_pb_api.h"

#if 0 //debug message control
#define printk printf
#else
#define printk ;/DBG_SLASH
#endif

extern UINT8 multiPlayEn;

void
sp1kDispPnlOpen(
	UINT8 panel
)
{
	devAttr_t* pDevAttr;
	UINT8 oldId;


	oldId = dispDevActGet();
	if ( panel == oldId ) {
		return;
	}

	/* check the new panel id first */
	if (dispDevActSet(panel) != SUCCESS) {
		return;
	} else {
		/* restore the active id */
		dispDevActSet(oldId);
	}

	dispPowerOff(oldId);
	dispDevActSet(panel);
	dispPowerOn(panel);

	pDevAttr = sp1kDevHdlGet();

	if ( panel > 1 ) {  /* tv. */
		pDevAttr[DEV_ID_TV].devSts = DEV_STS_RDY;
		pDevAttr[DEV_ID_TV].devEn = 1;
		pDevAttr[DEV_ID_PANEL].devEn = 0;
	}
	else {  /* panel. */
		pDevAttr[DEV_ID_PANEL].devSts = DEV_STS_RDY;
		pDevAttr[DEV_ID_TV].devEn = 0;
		pDevAttr[DEV_ID_PANEL].devEn = 1;
	}
}

/**
 * @fn UINT8 sp1kHalCtrlScreen(UINT8 attrib)
 * @brief	Set attributes to control screen.
 * @param[in] attrib	0: Show background color, no image, no OSD.
 *\n                    1: Show images, no OSD.
 *\n                    2: Show OSD, no images.
 *\n                    3: Show images and OSD.
 * @retval error code -  always reurn success(0)
 */
UINT8
sp1kHalCtrlScreen(
	UINT8 attrib
)
{
	HAL_DispParaSet(HAL_DISP_PARA_OSD_LAYER_ENABLE, (attrib & 0x02), 0, 0, 0, 0);
	HAL_DispParaSet(HAL_DISP_PARA_IMG_LAYER_ENABLE, (attrib & 0x01), 0, 0, 0, 0);
	
	if ( HAL_DramDispSrcGet() == HAL_DRAM_DISP_SRC_PB ) {
		HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PB | HAL_DRAM_ZF_UPD_LAYER_OSD);
		HAL_DramZUpdateWait(HAL_DRAM_ZUPDATE_PB, 0);
	}
	else {  /* HAL_DRAM_DISP_SRC_MPG. */
		HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PV | HAL_DRAM_ZF_UPD_LAYER_OSD);
		HAL_DramZUpdateWait(HAL_DRAM_ZUPDATE_PV, (UINT16)300);
		//printf("osd update\n");
	}

	return 0;
}

void
dispShow(
	UINT32 addr,
	UINT16 w,
	UINT16 h,
	UINT16 factor
)
{
	HAL_FrontParaSet(HAL_FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);

	/* clear pbb_reg_en & dispsrc_sel_en. */
	XBYTE[REG_Dram_MpgSb_RegIdx_En] &= ~0xA0;
	
	//For image disp use disp fit show. suny  S
#if 0
	/* set dispsrc_sel to pb buf. */
	HAL_DramDispSrcSel(0);
	HAL_DispPbFrameBufferSet(addr, w, h);
	HAL_DispScaleSet(w,h,factor, 0);

	HAL_DispParaSet(HAL_DISP_PARA_IMG_LAYER_ENABLE, 1, 0, 0, 0, 0);
	HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PB);
	HAL_DramZUpdateWait(HAL_DRAM_ZUPDATE_PB, 0);
#else
	HAL_DramDispSrcSel(0);
	HAL_DispPbFrameBufferSet(addr, w, h);	
	HAL_DispScaleSet(w, h, factor,0);

	#if 0
	/*still play back disp on tv not need to fit show 20110822*/
	if ((w == 720 && h == 480) || (w == 1440 && h == 960) || (w == 960 && h == 1440) || (w == 480 && h ==720)) {//TV NTSC
		if( (pbFile.wType == SP1K_PB_FILE_TYPE_AVI)||(pbFile.wType == SP1K_PB_FILE_TYPE_WGI) ){
			dispFitShow(w, h, 1);
		}	
	} else if ((w == 720 && h == 576) || (w == 1440 && h == 1152) ||(w == 1152 && h == 1440) || (w == 576 && h == 720)) {//TV PAL	
		;
	} else {
		dispFitShow(w, h, 1);
	}
	#endif
	
	dispFitShow(w, h, 1);
	#if 0
	if( multiPlayEn ){
		dispFitShow(w, h, 1);
	}
	#endif
	
	HAL_DispParaSet(HAL_DISP_PARA_IMG_LAYER_ENABLE, 1, 0, 0, 0, 0);
	HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PB);
	HAL_DramZUpdateWait(HAL_DRAM_ZUPDATE_PB, 0);	
#endif
//For image disp use disp fit show. suny  E

}

/**
 * @brief	set display fitting method.
 * @param	method = one of sp1kDispFitMethod_e.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-10-03
 * @todo	N/A
 * @bug		N/A
*/
void dispFitShow(UINT16 w, UINT16 h, UINT8 update)
{
	UINT16 h0, h1, v0, v1;
	UINT16 initW, initH;
	SINT16 hGap, vGap;
	UINT8 ratX, ratY;

	/* get panel init H&V */
	HAL_DispPnlInitHvGet(&h0, &h1, &v0, &v1);
    initW = h1 - h0 + 1;
    initH = v1 - v0 + 1;

	printk("old W: %lu, ", (UINT32)w);
	printk("old H: %lu\n", (UINT32)h);
	printk("initW: %lu, ", (UINT32)initW);
	printk("initH: %lu\n", (UINT32)initH);
	
	if (w == -1 || h == -1) {
	    /* restore the panel initialize setting */
	    hGap = 0;
	    vGap = 0;
	} else {
/*	
		if (initW == 720 && initH == 480 && 2 == dispDevActGet()) {//TV NTSC
			initW = 640;
			initH = 480;
			if(w < h){  // add for 16:9 rotation PB
				initW = 720;
				initH = 480;
			}
			if(XBYTE[REG_Disp_SclMode]&0x04){
				initW = 720;
				initH = 480;
			}
		} else if (initW == 720 && initH == 576 && 3 == dispDevActGet()) {//TV PAL	
			initW = 768;
			initH = 576;
			if(XBYTE[REG_Disp_SclMode]&0x04){
				initW = 720;
				initH = 576;
			}  
		} else
*/
		if (2 == dispDevActGet()) {
			initW = 704 * 10 / 11;
			
		} 
		else if (3 == dispDevActGet()) {
			initW = 704 * 12 / 11;
		}
		
		dispPnlAspectRatioGet(&ratX, &ratY);

		if (((UINT32)w * ratY) >= ((UINT32)ratX * h)) {
			/* fit width */
			h = ((UINT32)initH * h * ratX) / ((UINT32)w * ratY);
			w = initW;
		}
		else {
			/* fit height */
			w = ((UINT32)initW * w * ratY) / ((UINT32)h * ratX);
			h = initH;
		}
	
		//printf("new W: %lu,H: %lu\n ", (UINT32)w,(UINT32)h);
        if ((UINT32)((UINT32)w * initH) >= (UINT32)((UINT32)initW * h)) {
            /* fit width */
            hGap = 0;
            vGap = (initH - (((UINT32)h * initW) / (UINT32)w) + 1) >> 1;
        } 
		else {
            /* fit height */
            hGap = (initW - (((UINT32)w * initH) / (UINT32)h) + 1) >> 1;
            vGap = 0;
        }
	}

	//printf("gap H: %lu,V=%lu\n ", (UINT32)hGap,(UINT32)vGap);

	if (2 == dispDevActGet()) {
		hGap = hGap * 11 / 10;
		hGap += (720-704)>>1;
	} 
	else if (3 == dispDevActGet()) {
		hGap = hGap * 11 / 12;
		hGap += (720-702)>>1;
	}
	
	/* set data render region */
	HAL_DispImgHvSet(h0 + hGap, v0 + vGap, h1 - hGap, v1 - vGap);
	/* update scale factor */
	HAL_DispScaleUpdate();

	/* update display register */
	if (update) {
	    //HAL_DispRegUpdate();
	    HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PB | HAL_DRAM_ZF_UPD_LAYER_OSD);
	}
}

/**
 * @brief	set display fitting method.
 * @param	method = one of sp1kDispFitMethod_e.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-10-03
 * @todo	N/A
 * @bug		N/A
*/
void
sp1kDispFitMethodSet(
	UINT8 method
)
{
	HAL_DispFitMethodSet(method);
}

/**
 * @brief	enable/disable gamma.
 * @param	en = [in] 0: disable, 1: enable.
 * @retval	return = SUCCESS or FAIL.
 * @see
 * @author	Matt Wang
 * @since	2008-12-09
 * @todo	N/A
 * @bug		N/A
*/
UINT8
sp1kDispGammaEnableSet(
	UINT8 en
)
{
	return HAL_DispParaSet(HAL_DISP_PARA_GAMMA_ENABLE, en, 0, 0, 0, 2);
}

/**
 * @brief	set gamma table.
 * @param	gammaTbl = [in] pointer to gamma table, total 17 entries.
 * @retval	return = SUCCESS or FAIL.
 * @see
 * @author	Matt Wang
 * @since	2008-12-09
 * @todo	N/A
 * @bug		N/A
*/
UINT8
sp1kDispGammaTableSet(
	UINT8 *gammaTbl
)
{
	return HAL_DispGammaTableSet(gammaTbl);
}

/**
 * @fn        void sp1kDispOsdHvSet(UINT32 h0, UINT32 v0, UINT32 h1, UINT32 v1)
 * @brief     sp1kDispOsdHvSet
 * @param     [in] h0
 * @param     [in] v0
 * @param     [in] h1
 * @param     [in] v1
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-18
 * @todo      N/A
 * @bug       N/A
*/
void
sp1kDispOsdHvSet(
	UINT32 h0,
	UINT32 v0,
	UINT32 h1,
	UINT32 v1
)
{
	HAL_DispOsdHvSet(h0, v0, h1, v1);
}

/**
 * @fn        void sp1kDispOsdHvGet(UINT32 *h0, UINT32 *v0, UINT32 *h1, UINT32 *v1)
 * @brief     sp1kDispOsdHvGet
 * @param     [in] h0
 * @param     [in] v0
 * @param     [in] h1
 * @param     [in] v1
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-18
 * @todo      N/A
 * @bug       N/A
*/
void
sp1kDispOsdHvGet(
	UINT32 *h0,
	UINT32 *v0,
	UINT32 *h1,
	UINT32 *v1
)
{
	HAL_DispOsdHvGet(h0, v0, h1, v1);
}

/**
 * @fn        void sp1kDispOsdEnSet(UINT8 en)
 * @brief     sp1kDispOsdEnSet
 * @param     [in] en
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-18
 * @todo      N/A
 * @bug       N/A
*/
void
sp1kDispOsdEnSet(
	UINT8 en
)
{
	dispParaSet(DISP_PARA_OSD_LAYER_ENABLE, en?1:0, 0, 0, 0, 0);
	HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_OSD);
	
	XBYTE[REG_Disp_VsVdIntSts] |= 0x01;
	while(!(XBYTE[REG_Disp_VsVdIntSts] & 0x01));
}

/**
 * @fn        void sp1kDispOsdSclUpd(void)
 * @brief     update osd layer scale related parameter
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-18
 * @todo      N/A
 * @bug       N/A
*/
void
sp1kDispOsdSclUpd(
	void
)
{
	HAL_DispOsdScaleUpdate();
}

/**
 * @fn        void sp1kDispImgSclUpd(void)
 * @brief     update image layer scale related parameter
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-18
 * @todo      N/A
 * @bug       N/A
*/
void
sp1kDispImgSclUpd(
	void
)
{
	HAL_DispScaleUpdate();
}

