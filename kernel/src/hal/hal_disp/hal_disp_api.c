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
 **************************************************************************/
/**
 * @file		hal_disp.c
 * @brief		Hal API for DISPLAY module
 * @author		Ted Chiu, Matt Wang
 * @since		2008-03-14
 * @date		2008-10-31
 */

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "hal.h"
#include "hal_disp_internal.h"
#include "hal_disp.h"
#include "hal_gprm.h"
#include "hal_dram.h"
#include "disp_api.h"
#include "reg_def.h"
#include "dbg_def.h"
#include "hal_global.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
#define	HAL_DISP_PAN_DBG   0
#define	HAL_DISP_ZOOM_DBG  0

//#define HAL_DISP_WAIT_TIMEOUT  0xFFFF //xian move to hal_disp.h

/*
  pnlHSclFac = (PNLIMGH_IN - 1) * 4096 / (PNLIMGH_OUT - 1)
  pnlVSclFac = (PNLIMGV_IN - 1) * 8192 / (PNLIMGV_OUT - 1)
*/
#define HAL_DispScaleFacCalculate(srcH, srcV, dstH, dstV, facH, facV) \
{\
	(facH) = ((srcH) == (dstH)) ? 4096 : ((((srcH) - 1) << 12) / ((dstH) - 1));\
	if ((XBYTE[REG_Disp_FieldEnSwap] & 0x01) && ((XBYTE[REG_Disp_SclMode] & 0x04) == 0)) dstV >>= 1;\
	(facV) = ((srcV) == (dstV)) ? 8192 : ((((srcV) - 1) << 13) / ((dstV) - 1));\   
}

/*
  pnlHSclIni = ((PNLIMGH_IN - 1) * 4096 - pnlHSclFac * (PNLIMGH_OUT - 1)) / 2
  pnlVSclIni = ((PNLIMGV_IN - 1) * 8192 - pnlVSclFac * (PNLIMGV_OUT - 1)) / 2
*/
#define HAL_DispScaleIniCalculate(srcH, srcV, dstH, dstV, pnlHSclFac, pnlVSclFac, facIniH, facIniV) \
{\
	(facIniH) = ((((srcH) - 1) << 12) - (pnlHSclFac) * ((dstH) - 1)) >> 1;\
	(facIniV) = ((((srcV) - 1) << 13) - (pnlVSclFac) * ((dstV) - 1)) >> 1;\   
}

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
static UINT8 HAL_DispFitMethod = HAL_DISP_FIT_METHOD_SRC_STRETCH;
static UINT16 HAL_DispImgW = 0, HAL_DispImgH = 0;
static UINT32 ImgCurrX0, ImgCurrX1, ImgCurrY0, ImgCurrY1;
static UINT16 pnlInitH0, pnlInitH1, pnlInitV0, pnlInitV1;
static UINT16 HAL_DispPbFrameHeight;
/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/

void
HAL_Display_enable(
	void
)
{
	//printf("Display controller enabled\n");
}

void
HAL_DispRegUpdate(
	void
) USING_0
{
	//printf("disp upd\n");
	/* write disp_update. */
	XBYTE[REG_Disp_UpdPvOsd] |= 0x03;//image & osd layer 
}

/**
 * @brief	set fitting method.
 * @param	method = one of HAL_DispFitMethod_e.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-10-03
 * @todo	N/A
 * @bug		N/A
*/
void
HAL_DispFitMethodSet(
	UINT8 method
)
{
	if ( method >= HAL_DISP_FIT_METHOD_TOT ) {
		HAL_DispFitMethod = HAL_DISP_FIT_METHOD_SRC_STRETCH;
	}
	else {
		HAL_DispFitMethod = method;
	}
}

/**
 * @brief	shift image position on display (PAN)
 * @param	xoff, +/- value for current hort. position. unit:pixel
 * @param	yoff, +/- value for current vert. position. unit:pixel
 * @retval	err,0=OK,
 				bit0=X_UNDERFLOW, bit1=X_OVERFLOW,
 				bit2=Y_UNDERFLOW, bit3=Y_OVERFLOW.
 * @see
 * @author	Ted Chiu
 * @since	2008-03-19
 * @todo	N/A
 * @bug		N/A
*/
UINT8
HAL_DispImgPanSet(
	SINT16 xoff,
	SINT16 yoff
)
{
	UINT32 ImgX0, ImgX1, ImgY0, ImgY1;
	UINT32 shift_x, shift_y;
	UINT32 max_x, max_y;
	UINT8 err=0;
	UINT8 x=0,y=0;

	#if	HAL_DISP_PAN_DBG
	printf("HAL_DispImgPanSet()\n");
	#endif
	if ( HAL_DispImgW && HAL_DispImgH ) {
		HAL_DispImgXyGet(&ImgX0, &ImgY0, &ImgX1, &ImgY1);
		ImgX0 <<= 12;
		ImgY0 <<= 13;
		ImgX1 <<= 12;
		ImgY1 <<= 13;
		#if	HAL_DISP_PAN_DBG
		printf("pan old (%u.%u %u.%u) ~ (%u.%u %u.%u)\n", \
			(UINT16)(ImgX0>>12), (UINT16)((ImgX0 & 0x0FFF)*1000/4096),\
			(UINT16)(ImgY0>>13), (UINT16)((ImgY0 & 0x1FFF)*1000/8192),\
			(UINT16)(ImgX1>>12), (UINT16)((ImgX1 & 0x0FFF)*1000/4096),\
			(UINT16)(ImgY1>>13), (UINT16)((ImgY1 & 0x1FFF)*1000/8192));
		#endif

		/* calculate horizontal. */
		shift_x = (SINT32)xoff << 12;
		if((SINT32)(ImgX0+shift_x) < 0) {
			ImgX1 -= ImgX0;
			ImgX0  = 0;
			//err |= 0x01;
			x |= 0x01;
		}
		else {
			max_x = (UINT32)(HAL_DispImgW-1) << 12;
			if(ImgX1+shift_x > max_x) {
				ImgX0 += max_x - ImgX1;
				ImgX1  = max_x;
				//err |= 0x02;
				x |= 0x02;
			}
			else {
				ImgX0 += shift_x;
				ImgX1 += shift_x;
				if(ImgX0 == 0)
					x |= 0x01;
				if(ImgX1 == max_x)
					x |= 0x02;
					
			}
		}

		/* calculate vertical. */
		shift_y = (SINT32)yoff << 13;
		if((SINT32)(ImgY0+shift_y) < 0) {
			ImgY1 -= ImgY0;
			ImgY0  = 0;
			//err |= 0x04;
			y |= 0x04;
		}
		else {
			max_y = (UINT32)(HAL_DispImgH-1) << 13;
			if(ImgY1+shift_y > max_y) {
				ImgY0 += max_y - ImgY1;
				ImgY1  = max_y;
				//err |= 0x08;
				y |= 0x08;
			}
			else {
				ImgY0 += shift_y;
				ImgY1 += shift_y;
				if(ImgY0 == 0)
					y |= 0x04;
				if(ImgY1 == max_y)
					y |= 0x08;
			}
		}

		err = x | y;
		/* apply to hw. */
		HAL_DispImgXySet(ImgX0>>12, ImgY0>>13, ImgX1>>12, ImgY1>>13);
		#if	HAL_DISP_PAN_DBG
		
		printf("pan new (%u.%u %u.%u) ~ (%u.%u %u.%u) err=%bx\n", \
			(UINT16)(ImgX0>>12), (UINT16)((ImgX0 & 0x0FFF)*1000/4096),\
			(UINT16)(ImgY0>>13), (UINT16)((ImgY0 & 0x1FFF)*1000/8192),\
			(UINT16)(ImgX1>>12), (UINT16)((ImgX1 & 0x0FFF)*1000/4096),\
			(UINT16)(ImgY1>>13), (UINT16)((ImgY1 & 0x1FFF)*1000/8192), err);
		#endif
	}

	//HAL_DispRegUpdate();
	return err;
}

/**
 * @brief	set image zoom factor
 * @param	hsize, width of src image. unit:pixel
 * @param	vsize, high of src image. unit:pixel
 * @param	zoomfac, zoom factor. 1x=100,2x=200,...etc
 * @retval	err,0=OK,
 				bit0=X_UNDERFLOW, bit1=X_OVERFLOW,
 				bit2=Y_UNDERFLOW, bit3=Y_OVERFLOW.
 * @see
 * @author	Ted Chiu
 * @since	2010-01-25
 * @todo	N/A
 * @bug		N/A
*/

UINT8
HAL_DispScaleUpdate(
    void
)
{
    UINT32 pvX0, pvY0, pvX1, pvY1; 
    UINT32 pvH0, pvV0, pvH1, pvV1;
    UINT32 srcW, srcH, dispW, dispH;
    UINT32 pvHSclFac, pvVSclFac;
    UINT32 pvHSclIni, pvVSclIni;

    // filed en check the V size / 2
    // none scaling mode: keep the size of display buffer the same as TV V-size (480)

    HAL_DispImgXyGet(&pvX0, &pvY0, &pvX1, &pvY1);
    srcW = pvX1 - pvX0 + 1;
    srcH = pvY1 - pvY0 + 1;

    HAL_DispImgHvGet(&pvH0, &pvV0, &pvH1, &pvV1);
    dispW = pvH1 - pvH0 + 1;
    dispH = pvV1 - pvV0 + 1;
    
#if 0     
    printf("dispW: %lx\n", (UINT32)dispW);
    printf("dispH: %lx\n", (UINT32)dispH);
    printf("srcW: %lx\n", (UINT32)srcW);
    printf("srcH: %lx\n", (UINT32)srcH);
#endif    
	/*add 20111216,resolve black line show on tv in  pb multiplay mode*/
	if(XBYTE[REG_Disp_FieldEnSwap] & 0x01){
		if((dispW == srcW) && (dispH = srcH)){
			XBYTE[REG_Disp_SclMode] |= 0x04;
		}else{
			XBYTE[REG_Disp_SclMode] &= ~0x04 ;
		}
	}
	else{
		XBYTE[REG_Disp_SclMode] &= ~0x04 ;
	} 
	
    HAL_DispScaleFacCalculate(srcW, srcH, dispW, dispH, pvHSclFac, pvVSclFac);
    HAL_DispScaleIniCalculate(srcW, srcH, dispW, dispH, pvHSclFac, pvVSclFac, pvHSclIni, pvVSclIni);

	//printf("ini h=%lu,v=%lu\n",pvHSclIni, pvVSclIni);
    XBYTE[REG_Disp_PvHSclIniL] = READ8(pvHSclIni, 3);
    XBYTE[REG_Disp_PvHSclIniH] = READ8(pvHSclIni, 2);
    XBYTE[REG_Disp_PvVSclIniL] = READ8(pvVSclIni, 3);
    XBYTE[REG_Disp_PvVSclIniH] = READ8(pvVSclIni, 2);
    
    XBYTE[REG_Disp_PvHSclFctL] = READ8(pvHSclFac, 3);
    XBYTE[REG_Disp_PvHSclFctH] = READ8(pvHSclFac, 2);
    XBYTE[REG_Disp_PvVSclFctL] = READ8(pvVSclFac, 3);
    XBYTE[REG_Disp_PvVSclFctH] = READ8(pvVSclFac, 2);
    
#if 0
    printf("pvX0:%lx, pvY0:%lx, pvX1:%lx, pvY1:%lx\n", pvX0, pvY0, pvX1, pvY1);
    printf("pvH0:%lx, pvV0:%lx, pvH1:%lx, pvV1:%lx\n", pvH0, pvV0, pvH1, pvV1);
    printf("pvHSclIni:%lx, pvVSclIni:%lx, pvHSclFac:%lx, pvVSclFac:%lx\n", pvHSclIni, pvVSclIni, pvHSclFac, pvVSclFac);
#endif

    return 0;
}

/**
 * @fn        UINT8 HAL_DispOsdScaleUpdate(void)
 * @brief     HAL_DispOsdScaleUpdate
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-18
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_DispOsdScaleUpdate(
 void
)
{
	UINT32 osdX0, osdY0, osdX1, osdY1; 
	UINT32 osdH0, osdV0, osdH1, osdV1;
	UINT32 srcW, srcH, dispW, dispH;
	UINT32 osdHSclFac, osdVSclFac;
	UINT32 osdHSclIni, osdVSclIni;

	/* get osdx0 */
	WRITE8(osdX0, 3, XBYTE[REG_Disp_OsdX0L]);
	WRITE8(osdX0, 2, XBYTE[REG_Disp_OsdX0H]);
	WRITE8(osdX0, 1, 0);
	WRITE8(osdX0, 0, 0);

	/* get osdx1 */
	WRITE8(osdX1, 3, XBYTE[REG_Disp_OsdX1L]);
	WRITE8(osdX1, 2, XBYTE[REG_Disp_OsdX1H]);
	WRITE8(osdX1, 1, 0);
	WRITE8(osdX1, 0, 0);

	/* get osdy0 */
	WRITE8(osdY0, 3, XBYTE[REG_Disp_OsdY0L]);
	WRITE8(osdY0, 2, XBYTE[REG_Disp_OsdY0H]);
	WRITE8(osdY0, 1, 0);
	WRITE8(osdY0, 0, 0);

	/* get osdy1 */
	WRITE8(osdY1, 3, XBYTE[REG_Disp_OsdY1L]);
	WRITE8(osdY1, 2, XBYTE[REG_Disp_OsdY1H]);
	WRITE8(osdY1, 1, 0);
	WRITE8(osdY1, 0, 0);

	srcW = osdX1 - osdX0 + 1;
	srcH = osdY1 - osdY0 + 1;

	/* get osdH0 */
	WRITE8(osdH0, 3, XBYTE[REG_Disp_OsdH0L]);
	WRITE8(osdH0, 2, XBYTE[REG_Disp_OsdH0H]);
	WRITE8(osdH0, 1, 0);
	WRITE8(osdH0, 0, 0);

	/* get osdH1 */
	WRITE8(osdH1, 3, XBYTE[REG_Disp_OsdH1L]);
	WRITE8(osdH1, 2, XBYTE[REG_Disp_OsdH1H]);
	WRITE8(osdH1, 1, 0);
	WRITE8(osdH1, 0, 0);

	/* get osdV0 */
	WRITE8(osdV0, 3, XBYTE[REG_Disp_OsdV0L]);
	WRITE8(osdV0, 2, XBYTE[REG_Disp_OsdV0H]);
	WRITE8(osdV0, 1, 0);
	WRITE8(osdV0, 0, 0);

	/* get osdV1 */
	WRITE8(osdV1, 3, XBYTE[REG_Disp_OsdV1L]);
	WRITE8(osdV1, 2, XBYTE[REG_Disp_OsdV1H]);
	WRITE8(osdV1, 1, 0);
	WRITE8(osdV1, 0, 0);

	dispW = osdH1 - osdH0 + 1;
	dispH = osdV1 - osdV0 + 1;
	HAL_DispScaleFacCalculate(srcW, srcH, dispW, dispH, osdHSclFac, osdVSclFac);
	HAL_DispScaleIniCalculate(srcW, srcH, dispW, dispH, osdHSclFac, osdVSclFac, osdHSclIni, osdVSclIni);

	XBYTE[REG_Disp_OsdHSclIniL] = READ8(osdHSclIni, 3);
	XBYTE[REG_Disp_OsdHSclIniH] = READ8(osdHSclIni, 2);
	XBYTE[REG_Disp_OsdVSclIniL] = READ8(osdVSclIni, 3);
	XBYTE[REG_Disp_OsdVSclIniH] = READ8(osdVSclIni, 2);

	XBYTE[REG_Disp_OsdHSclFctL] = READ8(osdHSclFac, 3);
	XBYTE[REG_Disp_OsdHSclFctH] = READ8(osdHSclFac, 2);
	XBYTE[REG_Disp_OsdVSclFctL] = READ8(osdVSclFac, 3);
	XBYTE[REG_Disp_OsdVSclFctH] = READ8(osdVSclFac, 2);

#if 0
	printf("osdX0:%lx, osdY0:%lx, osdX1:%lx, osdY1:%lx\n", osdX0, osdY0, osdX1, osdY1);
	printf("osdH0:%lx, osdV0:%lx, osdH1:%lx, osdV1:%lx\n", osdH0, osdV0, osdH1, osdV1);
	printf("osdHSclIni:%lx, osdVSclIni:%lx, osdHSclFac:%lx, osdVSclFac:%lx\n", osdHSclIni, osdVSclIni, osdHSclFac, osdVSclFac);
#endif

	return 0;
}

/**
 * @brief	set image zoom factor
 * @param	hsize, width of src image. unit:pixel
 * @param	vsize, high of src image. unit:pixel
 * @param	zoomfac, zoom factor. 1x=100,2x=200,...etc
 * @retval	err,0=OK,
 				bit0=X_UNDERFLOW, bit1=X_OVERFLOW,
 				bit2=Y_UNDERFLOW, bit3=Y_OVERFLOW.
 * @see
 * @author	Ted Chiu
 * @since	2010-01-25
 * @todo	N/A
 * @bug		N/A
*/
#if 0
UINT8
HAL_DispScaleSet(
	UINT16 imgW,
	UINT16 imgH,
	UINT16 zoomfac
)
{
    imgW = imgW;
    imgH = imgH;
}
#else
UINT8
HAL_DispScaleSet(
	UINT16 imgW,
	UINT16 imgH,
	UINT16 zoomfac,
	UINT8 update
)
{
    UINT16 zfacX, zfacY;
    UINT16 dispW, dispH;
    UINT32 pnlX0, pnlY0, pnlX1, pnlY1;
	UINT32 imgX0, imgX1, imgY0, imgY1;
	UINT32 CurrXc, CurrYc;
	UINT32 tmp32;
	UINT8  err = 0;
	UINT32 dstSrcW, dstSrcH;

	#if	HAL_DISP_ZOOM_DBG
	printf("HAL_DispScaleSet()\n");
	#endif

	HAL_DispImgXyGet(&pnlX0, &pnlY0, &pnlX1, &pnlY1);

	dispW = pnlX1 - pnlX0 + 1;
	dispH = pnlY1 - pnlY0 + 1;

	imgX0 = 0;
	imgX1 = (UINT32)(imgW-1) << 12; // W*4096
	imgY0 = 0;
	imgY1 = (UINT32)(imgH-1) << 13; // H*8192

	zfacX = zoomfac;
	zfacY = zoomfac;
	if ( HAL_DispFitMethod == HAL_DISP_FIT_METHOD_DST_OUTSIDE ) {
		dstSrcW = (UINT32)imgW * dispH;
		dstSrcH = (UINT32)imgH * dispW;
		#if	HAL_DISP_ZOOM_DBG
		printf("dsw=%ld dsh=%ld\n", dstSrcW, dstSrcH);
		#endif

		if ( dstSrcW < dstSrcH ) {  /* portrait. */
			zfacY = (UINT32)zoomfac * (dstSrcH / imgW) / dispW;
		}
		else if ( dstSrcW > dstSrcH ) {  /* landscape. */
			zfacX = (UINT32)zoomfac * (dstSrcW / imgH) / dispH;
		}
	} else {
		#if 0 //linjie.cheng fixed check by feng.lin
		/* only support 4:3 */
		dstSrcW = 320;
		dstSrcH = 240;
		#else
		/* support 4:3 and 16:9 */
		dstSrcW = 320;		
		dstSrcH = dstSrcW / imgW * imgH;
		#endif
	}

	if (zfacX != 100 && imgW == HAL_DispImgW) {
		CurrXc = (pnlX0 + pnlX1 + 1) << (12-1);
	} 
	else {
		CurrXc = imgX1 >> 1;
	}
	
	if (zfacY != 100 && imgH == HAL_DispImgH) {
		CurrYc = (pnlY0 + pnlY1 + 1) << (13-1);
	} 
	else {
		CurrYc = imgY1 >> 1;
	}

	HAL_DispImgW = imgW;
	HAL_DispImgH = imgH;

	#if	HAL_DISP_ZOOM_DBG
	printf("dw=%d dh=%d iw=%d ih=%d zfx=%d zfy=%d\n",
	       dispW, dispH, imgW, imgH, zfacX, zfacY);
	printf("zoom old (%u.%u %u.%u) ~ (%u.%u %u.%u)\n",
		(UINT16)(imgX0>>12), (UINT16)((imgX0 & 0x0FFF)*1000/4096),
		(UINT16)(imgY0>>13), (UINT16)((imgY0 & 0x1FFF)*1000/8192),
		(UINT16)(imgX1>>12), (UINT16)((imgX1 & 0x0FFF)*1000/4096),
		(UINT16)(imgY1>>13), (UINT16)((imgY1 & 0x1FFF)*1000/8192));
	#endif

	/* calculate horizontal. */
	
	tmp32 = (((UINT32)imgW * 100) << (12-1)) / zfacX;
	zfacX = (tmp32 << 1) / dispW;
	if ( dstSrcW > dstSrcH || zfacX != 100 ) {
    	imgX0 = CurrXc - tmp32;
    	imgX1 = CurrXc + tmp32-1;//hy.ren 2012-09-07
	}

	#if	HAL_DISP_ZOOM_DBG
	printf("src w/2=%u.%u zfacX=%04X x=(%u.%u %u.%u)\n",
		(UINT16)(tmp32>>12), (UINT16)((tmp32 & 0x0FFF)*1000/4096),zfacX,
		(UINT16)(imgX0>>12), (UINT16)((imgX0 & 0x0FFF)*1000/4096),
		(UINT16)(imgX1>>12), (UINT16)((imgX1 & 0x0FFF)*1000/4096));
	#endif

	/* calculate vertical. */

	tmp32 = (((UINT32)imgH * 100) << (13-1)) / zfacY;
	zfacY = (tmp32 << 1) / dispH;
	if ( dstSrcW < dstSrcH || zfacY != 100 ) {
		imgY0 = CurrYc - tmp32;
		imgY1 = CurrYc + tmp32-1; //hy.ren 2012-09-07
	}

	#if	HAL_DISP_ZOOM_DBG
	printf("src h/2=%u.%u zfacY=%04X y=(%u.%u %u.%u)\n", \
		(UINT16)(tmp32>>13), (UINT16)((tmp32 & 0x1FFF)*1000/8192),zfacY,
		(UINT16)(imgY0>>13), (UINT16)((imgY0 & 0x1FFF)*1000/8192),
		(UINT16)(imgY1>>13), (UINT16)((imgY1 & 0x1FFF)*1000/8192));
	#endif

	/* boundary check. */
	tmp32 = (UINT32)(imgW-1) << 12;
	if((SINT32)imgX0 < 0) {	/* 0 ~ x1-x0, MinUpperBoundary */
		err |= 0x01;
		imgX1 = imgX1 - imgX0;
		if(imgX1 > tmp32) {
			imgX1 = tmp32;
		}
		imgX0 = 0;
	}
	if(imgX1 > tmp32) { /* max-1-(x1-x0) ~ max-1, MaxLowerBoundary */
		err |= 0x02;
		imgX0 = tmp32 - (imgX1-imgX0);
		if((SINT32)imgX0 < 0) {
			imgX0 = 0;
		}
		imgX1 = tmp32;
	}

	tmp32 = (UINT32)(imgH-1) << 13;
	if((SINT32)imgY0 < 0) {	/* 0 ~ y1-y0, MinUpperBoundary */
		err |= 0x04;
		imgY1 = imgY1 - imgY0;
		if(imgY1 > tmp32) {
			imgY1 = tmp32;
		}
		imgY0 = 0;
	}
	if(imgY1 > tmp32) {	/* max-1-(y1-y0) ~ max-1, MaxLowerBoundary */
		err |= 0x08;
		imgY0 = tmp32 - (imgY1-imgY0);
		if((SINT32)imgY0 < 0) {
			imgY0 = 0;
		}
		imgY1 = tmp32;
	}
	
	#if	HAL_DISP_ZOOM_DBG
	printf("zoom new (%u.%u %u.%u) ~ (%u.%u %u.%u) err=%02bX\n", \
			(UINT16)(imgX0>>12), (UINT16)((imgX0 & 0x0FFF)*1000/4096),\
			(UINT16)(imgY0>>13), (UINT16)((imgY0 & 0x1FFF)*1000/8192),\
			(UINT16)(imgX1>>12), (UINT16)((imgX1 & 0x0FFF)*1000/4096),\
			(UINT16)(imgY1>>13), (UINT16)((imgY1 & 0x1FFF)*1000/8192), err);
	#endif

	/* apply to hw. */
	HAL_DispImgXySet(imgX0>>12, imgY0>>13, imgX1>>12, imgY1>>13);

	HAL_DispScaleUpdate();

	if (update){
		HAL_DispRegUpdate();
	}

    return err;
}
#endif
/**
 * @brief	get source image layer cropping region
 * @param	*x0 = [out] start x.
 * @param	*y0 = [out] start y.
 * @param	*x1 = [out] end x.
 * @param	*y1 = [out] end y.
 * @retval	None.
 * @see
 * @author	Phil Lin
 * @since	2010-03-12
 * @todo	N/A
 * @bug		N/A
*/
void
HAL_DispImgXyGet(
	UINT32 *x0,
	UINT32 *y0,
	UINT32 *x1,
	UINT32 *y1
)
{
	*x0 = (UINT16)XBYTE[REG_Disp_PvX0L] | ((UINT16)XBYTE[REG_Disp_PvX0H] << 8);
	*x1 = (UINT16)XBYTE[REG_Disp_PvX1L] | ((UINT16)XBYTE[REG_Disp_PvX1H] << 8);
	*y0 = (UINT16)XBYTE[REG_Disp_PvY0L] | ((UINT16)XBYTE[REG_Disp_PvY0H] << 8);
	*y1 = (UINT16)XBYTE[REG_Disp_PvY1L] | ((UINT16)XBYTE[REG_Disp_PvY1H] << 8);
}

/**
 * @brief	set source image layer cropping region
 * @param	x0 = [in] start x.
 * @param	y0 = [in] start y.
 * @param	x1 = [in] end x.
 * @param	y1 = [in] end y.
 * @retval	None.
 * @see
 * @author	Phil Lin
 * @since	2010-03-12
 * @todo	N/A
 * @bug		N/A
*/
void
HAL_DispImgXySet(
	UINT32 x0,
	UINT32 y0,
	UINT32 x1,
	UINT32 y1
)
{
    XBYTE[REG_Disp_PvX0L] = READ8(x0, 3);
    XBYTE[REG_Disp_PvX0H] = READ8(x0, 2);

    XBYTE[REG_Disp_PvX1L] = READ8(x1, 3);
    XBYTE[REG_Disp_PvX1H] = READ8(x1, 2);

    XBYTE[REG_Disp_PvY0L] = READ8(y0, 3);
    XBYTE[REG_Disp_PvY0H] = READ8(y0, 2);

    XBYTE[REG_Disp_PvY1L] = READ8(y1, 3);
    XBYTE[REG_Disp_PvY1H] = READ8(y1, 2);   
}

/**
 * @brief	get image data render region
 * @param	*h0 = [out] start h.
 * @param	*v0 = [out] start v.
 * @param	*h1 = [out] end h.
 * @param	*v1 = [out] end v.
 * @retval	None.
 * @see
 * @author	Phil Lin
 * @since	2010-03-12
 * @todo	N/A
 * @bug		N/A
*/
void
HAL_DispImgHvGet(
	UINT32 *h0,
	UINT32 *v0,
	UINT32 *h1,
	UINT32 *v1
)
{
	*h0 = (UINT16)XBYTE[REG_Disp_PvH0L] | ((UINT16)XBYTE[REG_Disp_PvH0H] << 8);
	*h1 = (UINT16)XBYTE[REG_Disp_PvH1L] | ((UINT16)XBYTE[REG_Disp_PvH1H] << 8);
	*v0 = (UINT16)XBYTE[REG_Disp_PvV0L] | ((UINT16)XBYTE[REG_Disp_PvV0H] << 8);
	*v1 = (UINT16)XBYTE[REG_Disp_PvV1L] | ((UINT16)XBYTE[REG_Disp_PvV1H] << 8);
}

/**
 * @brief	set image data render region
 * @param	h0 = [in] start h.
 * @param	v0 = [in] start v.
 * @param	h1 = [in] end h.
 * @param	v1 = [in] end v.
 * @retval	None.
 * @see
 * @author	Phil Lin
 * @since	2010-03-12
 * @todo	N/A
 * @bug		N/A
*/
void
HAL_DispImgHvSet(
	UINT32 h0,
	UINT32 v0,
	UINT32 h1,
	UINT32 v1
)
{
    XBYTE[REG_Disp_PvH0L] = READ8(h0, 3);
    XBYTE[REG_Disp_PvH0H] = READ8(h0, 2);

    XBYTE[REG_Disp_PvH1L] = READ8(h1, 3);
    XBYTE[REG_Disp_PvH1H] = READ8(h1, 2);

    XBYTE[REG_Disp_PvV0L] = READ8(v0, 3);
    XBYTE[REG_Disp_PvV0H] = READ8(v0, 2);

    XBYTE[REG_Disp_PvV1L] = READ8(v1, 3);
    XBYTE[REG_Disp_PvV1H] = READ8(v1, 2);   
}

/**
 * @fn        void HAL_DispOsdHvGet(UINT32 *h0, UINT32 *v0, UINT32 *h1, UINT32 *v1)
 * @brief     HAL_DispOsdHvGet
 * @param     [in] h0
 * @param     [in] v0
 * @param     [in] h1
 * @param     [in] v1
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-12-28
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispOsdHvGet(
	UINT32 *h0,
	UINT32 *v0,
	UINT32 *h1,
	UINT32 *v1
)
{
	*h0 = (UINT16)XBYTE[REG_Disp_OsdH0L] | ((UINT16)XBYTE[REG_Disp_OsdH0H] << 8);
	*h1 = (UINT16)XBYTE[REG_Disp_OsdH1L] | ((UINT16)XBYTE[REG_Disp_OsdH1H] << 8);
	*v0 = (UINT16)XBYTE[REG_Disp_OsdV0L] | ((UINT16)XBYTE[REG_Disp_OsdV0H] << 8);
	*v1 = (UINT16)XBYTE[REG_Disp_OsdV1L] | ((UINT16)XBYTE[REG_Disp_OsdV1H] << 8);
}

/**
 * @fn        void HAL_DispOsdHvSet(UINT32 h0, UINT32 v0, UINT32 h1, UINT32 v1)
 * @brief     HAL_DispOsdHvSet
 * @param     [in] h0
 * @param     [in] v0
 * @param     [in] h1
 * @param     [in] v1
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-12-28
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispOsdHvSet(
	UINT32 h0,
	UINT32 v0,
	UINT32 h1,
	UINT32 v1
)
{
    XBYTE[REG_Disp_OsdH0L] = READ8(h0, 3);
    XBYTE[REG_Disp_OsdH0H] = READ8(h0, 2);

    XBYTE[REG_Disp_OsdH1L] = READ8(h1, 3);
    XBYTE[REG_Disp_OsdH1H] = READ8(h1, 2);

    XBYTE[REG_Disp_OsdV0L] = READ8(v0, 3);
    XBYTE[REG_Disp_OsdV0H] = READ8(v0, 2);

    XBYTE[REG_Disp_OsdV1L] = READ8(v1, 3);
    XBYTE[REG_Disp_OsdV1H] = READ8(v1, 2);   
}

/**
 * @brief	get panel active data region
 * @param	*h0 = [out] start h.
 * @param	*v0 = [out] start v.
 * @param	*h1 = [out] end h.
 * @param	*v1 = [out] end v.
 * @retval	None.
 * @see
 * @author	Phil Lin
 * @since	2010-03-12
 * @todo	N/A
 * @bug		N/A
*/
void
HAL_DispPnlInitHvGet(
	UINT16 *h0,
	UINT16 *h1,
	UINT16 *v0,
	UINT16 *v1
)
{
	*h0 = pnlInitH0;
	*h1 = pnlInitH1;
	*v0 = pnlInitV0;
	*v1 = pnlInitV1;
}

/**
 * @brief	set panel active data region
 * @param	h0 = [in] start h.
 * @param	v0 = [in] start v.
 * @param	h1 = [in] end h.
 * @param	v1 = [in] end v.
 * @retval	None.
 * @see
 * @author	Phil Lin
 * @since	2010-03-12
 * @todo	N/A
 * @bug		N/A
*/
void
HAL_DispPnlInitHvSet(
	UINT16 h0,
	UINT16 h1,
	UINT16 v0,
	UINT16 v1
)
{
    pnlInitH0 = h0; 
    pnlInitH1 = h1; 
    pnlInitV0 = v0; 
    pnlInitV1 = v1; 
}
#if 0
#endif

/**
 * @brief	set pb frame buffer.
 * @param	addr = [in] frame buffer address.
 * @param	w = [in] frame buffer width.
 * @param	h = [in] frame buffer height.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-07-16
 * @todo	N/A
 * @bug		N/A
*/
void
HAL_DispPbFrameBufferSet(
	UINT32 addr,
	UINT16 w,
	UINT16 h
)
{
	/* set PBBaddr. */
	XBYTE[REG_Dram_PBBAddr_B0] = READ8(addr, 3);
	XBYTE[REG_Dram_PBBAddr_B1] = READ8(addr, 2);
	XBYTE[REG_Dram_PBBAddr_B2] = READ8(addr, 1);
	XBYTE[REG_Dram_PBBAddr_B3] = READ8(addr, 0);

	/* set PBBhsize. */
	XBYTE[REG_Dram_PBBHSize_Low] = READ8(w, 1);
	XBYTE[REG_Dram_PBBHSize_High] = READ8(w, 0);

	HAL_DispPbFrameHeight = h;
}

/**
 * @brief	get pb frame buffer.
 * @param	*paddr = [out] frame buffer address.
 * @param	*pw = [out] frame buffer width.
 * @param	*ph = [out] frame buffer height.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-07-16
 * @todo	N/A
 * @bug		N/A
*/
void
HAL_DispPbFrameBufferGet(
	UINT32 *paddr,
	UINT16 *pw,
	UINT16 *ph
)
{
	if ( paddr != NULL ) {
		*((UINT8*)paddr + 3) = XBYTE[REG_Dram_PBBAddr_B0];
		*((UINT8*)paddr + 2) = XBYTE[REG_Dram_PBBAddr_B1];
		*((UINT8*)paddr + 1) = XBYTE[REG_Dram_PBBAddr_B2];
		*((UINT8*)paddr + 0) = XBYTE[REG_Dram_PBBAddr_B3];
	}

	if ( pw != NULL ) {
		*((UINT8*)pw + 1) = XBYTE[REG_Dram_PBBHSize_Low];
		*((UINT8*)pw + 0) = XBYTE[REG_Dram_PBBHSize_High];
	}

	if ( ph != NULL ) {
		*ph = HAL_DispPbFrameHeight;
	}
}

#if 0
#endif

UINT16 gDispOsdBuffSizeW;
UINT16 gDispOsdBuffSizeH;
/**
 * @fn        void HAL_DispOsdBuffAddrSet(UINT32 addrWord)
 * @brief     set osd font buffer address
 * @param     [in] addrWord
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispOsdBuffAddrSet(
	UINT32 addrWord
)
{
	/* set OSDaddr. */
	XBYTE[REG_Dram_OSDAddr_B0] = READ8(addrWord, 3);
	XBYTE[REG_Dram_OSDAddr_B1] = READ8(addrWord, 2);
	XBYTE[REG_Dram_OSDAddr_B2] = READ8(addrWord, 1);
	XBYTE[REG_Dram_OSDAddr_B3] = READ8(addrWord, 0);
}

/**
 * @fn        void HAL_DispOsdBuffSizeSet(UINT16 buffW, UINT16 buffH)
 * @brief     HAL_DispOsdBuffSizeSet
 * @param     [in] buffW
 * @param     [in] buffH
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispOsdBuffSizeSet(
	UINT16 buffW,
	UINT16 buffH
)
{
	buffW >>= 1; //byte to word
	
	/* set buffer width. */
	XBYTE[REG_Dram_OSDHSize_Low] = READ8(buffW, 1);  /* always be 1024. */
	XBYTE[REG_Dram_OSDHSize_High] = READ8(buffW, 0);
	
	gDispOsdBuffSizeW = buffW;
	gDispOsdBuffSizeH = buffH;
}

void
HAL_DispOsdBuffSizeGet(
	UINT16* pBuffW,
	UINT16* pBuffH
)
{
#if TAG_TODO
	*pBuffW = gDispOsdBuffSizeW;
	*pBuffH = gDispOsdBuffSizeH;
#else
	*pBuffW = 320;
	*pBuffH = 240;
#endif
}

/**
 * @fn        void HAL_DispOsdBuffAddrGet(UINT32 *pAddrWord)
 * @brief     get osd font buffer
 * @param     [in] pAddrWord
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispOsdBuffAddrGet(
	UINT32 *pAddrWord
)
{
	if ( pAddrWord != NULL ) {
		*((UINT8*)pAddrWord + 3) = XBYTE[REG_Dram_OSDAddr_B0];
		*((UINT8*)pAddrWord + 2) = XBYTE[REG_Dram_OSDAddr_B1];
		*((UINT8*)pAddrWord + 1) = XBYTE[REG_Dram_OSDAddr_B2];
		*((UINT8*)pAddrWord + 0) = XBYTE[REG_Dram_OSDAddr_B3];
	}
}

/**
 * @fn        UINT8 HAL_DispOsdPaletteSet(UINT16 *paltTbl, UINT16 begIdx, UINT16 len)
 * @brief     HAL_DispOsdPaletteSet
 * @param     [in] paltTbl
 * @param     [in] begIdx
 * @param     [in] len
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-19
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_DispOsdPaletteSet(
	UINT16 *paltTbl,
	UINT16 begIdx,
	UINT16 len
)
{
//	UINT8 *tblSrc;
	UINT16 idx, max;


	/* check validation. */
	max = begIdx + len;
	if ( begIdx >= 64 || max > 64 ) {
		return FAIL;
	}

    /* enable color look up table update */
    XBYTE[REG_Disp_CpuSramEn] = 0x01;

	for (idx = begIdx; idx < max; idx++) {
	    /* set color index */
	    XBYTE[REG_Disp_CpuSramAddr] = idx; 

	    XBYTE[REG_Disp_CpuSramDatH] = READP8(paltTbl, 1); // little endian
	    XBYTE[REG_Disp_CpuSramDatL] = READP8(paltTbl, 0);;
	}

    /* disable color look up table update */
    XBYTE[REG_Disp_CpuSramEn] = 0x00;
    
	return SUCCESS;
}
/**
 * @brief	osd color palette init.
 * @param	address = data address (dram linear address / pointer address).
 * @param   data mode: 0:code mode / 1:resoure in dram
 * @retval	return = SUCCESS or FAIL.
 * @see
 * @author	Phil Lin
 * @since	2010-02-01
 * @todo	N/A
 * @bug		N/A
*/
UINT8
HAL_DispOsdPaletteInit(
	UINT32 addr,
	UINT8 dataMode
)
{
    UINT8 i8;
    UINT8 xdata* ptr;
    UINT8 LSB, MSB;

    if (dataMode) {
        HAL_DramStartAddrSet(addr, 1);
    } else {
        ptr = (UINT8 xdata*)addr;
    }

    /* enable color look up table update */
    XBYTE[REG_Disp_CpuSramEn] = 0x01;
    
    for (i8 = 0; i8 < 64; i8++) {
        /* set color index */
        XBYTE[REG_Disp_CpuSramAddr] = i8; 

        /* fill data */
        if (dataMode) {
            HAL_DramWordDataRead(&LSB, &MSB); // big endian
            XBYTE[REG_Disp_CpuSramDatL] = LSB;
            XBYTE[REG_Disp_CpuSramDatH] = MSB;
        } else {
            //printf("ptr: %lx\n", (UINT32)(*(UINT16 xdata*)ptr));
            XBYTE[REG_Disp_CpuSramDatH] = *ptr++; // little endian
            XBYTE[REG_Disp_CpuSramDatL] = *ptr++;
        }
    }
    
    /* disable color look up table update */
    XBYTE[REG_Disp_CpuSramEn] = 0x00;

    return SUCCESS;
}

#if 0
#endif

/**
 * @brief	set gamma table.
 * @param	gammaTbl = [in] pointer to gamma table, total 17 entries.
 * @retval	return = SUCCESS or FAIL.
 * @see
 * @author	Matt Wang
 * @since	2008-05-26
 * @todo	N/A
 * @bug		N/A
*/
UINT8
HAL_DispGammaTableSet(
	UINT8 *gammaTbl
)
{
	UINT8 *tblSrc;
	UINT16 idx, max;


	/* write osd palette. */
	tblSrc = gammaTbl;
	idx = 0x2D60;
	max = 0x2D60 + 17;

	while ( idx < max ) {
		/* write each entry. */
		XBYTE[idx] = *tblSrc;
		idx++;
		tblSrc++;
	}

	return SUCCESS;
}

#if 0
#endif

/**
 * @brief	wait for signal event.
 * @param	src = [in] one of HAL_DispWaitSource_e.
 * @param	mode = [in] one of HAL_DispWaitMode_e.
 * @retval	return = SUCCESS or FAIL.
 * @see
 * @author	Matt Wang
 * @since	2010-01-19
 * @todo	N/A
 * @bug		N/A
*/
UINT8
HAL_DispSignalWait(
	UINT8 src,
	UINT8 mode
)
{
	UINT16 timeout;


	/* if dispEn or dispPhaseClkEn disabled, no need to wait. */
	if ( !(XBYTE[REG_Disp_DispEn_Flip] & 0x01) || !(XBYTE[0x2013] & 0x10) ) {
		return FAIL;
	}

	/* wait for specified event. */
	timeout = HAL_DISP_WAIT_TIMEOUT;

	while ( 1 ) {
		if ( XBYTE[REG_Disp_DispSignal] & src ) {  /* high detected */
			if ( mode == HAL_DISP_WAIT_FALLING ) {
				break;
			}
			if ( mode == HAL_DISP_WAIT_HIGH ) {
				return SUCCESS;
			}
		}
		else {  /* !(XBYTE[REG_Disp_DispSignal] & src) */  /* low detected */
			if ( mode == HAL_DISP_WAIT_RISING ) {
				break;
			}
			if ( mode == HAL_DISP_WAIT_LOW ) {
				return SUCCESS;
			}
		}

		timeout--;
		if ( timeout == 0 ) {
			return FAIL;
		}
	}

	while ( 1 ) {
		if ( !(XBYTE[REG_Disp_DispSignal] & src) ) {  /* low detected */
			if ( mode == HAL_DISP_WAIT_FALLING ) {
				return SUCCESS;
			}
		}
		else {  /* XBYTE[REG_Disp_DispSignal] & src */  /* high detected */
			if ( mode == HAL_DISP_WAIT_RISING ) {
				return SUCCESS;
			}
		}

		timeout--;
		if ( timeout == 0 ) {
			return FAIL;
		}
	}
}

void HAL_DispImgXyUpdate(UINT16 newImgW, UINT16 newImgH)
{
	UINT32 pnlX0, pnlX1, pnlY0, pnlY1;
	
	if(HAL_DispImgW && HAL_DispImgH){
		HAL_DispImgXyGet(&pnlX0, &pnlY0, &pnlX1, &pnlY1);
		pnlX0 = pnlX0 * newImgW / HAL_DispImgW;
		pnlX1 = pnlX1 * newImgW / HAL_DispImgW;
		pnlY0 = pnlY0 * newImgH / HAL_DispImgH;
		pnlY1 = pnlY1 * newImgH / HAL_DispImgH; 				
		HAL_DispImgXySet(pnlX0, pnlY0, pnlX1, pnlY1);
		HAL_DispImgW = newImgW;
		HAL_DispImgH = newImgH;
	}
}

void HAL_DispImgSizeGet(UINT16 *width,UINT16 *heigth)
{
	*width = HAL_DispImgW;
	*heigth=HAL_DispImgH;
}


