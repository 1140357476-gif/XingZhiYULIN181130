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
* @file      hal_disp_paraapi.c
* @brief     disp parameter set related functions
* @author    Phil Lin
* @since     2010-01-01
* @date      2010-01-01
*/

/**************************************************************************
*                        H E A D E R   F I L E S
**************************************************************************/
#include "general.h"
#include "hal_disp.h"
#include "hal_disp_internal.h"
#include "reg_def.h"

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

static lcmSetItem_t xdata sLcmSetItem[] = {

	/* data format of "8 Bit" and "RGB444" */

	/* lcmBusW lcmMode lcmCyc pnlRBChg pnlGChg pktMode bigEnd midEnd */
	{  0,	   2,	   3,	  0,	   0,	   0,	   0,	  0 },
	{  0,	   2,	   3,	  0,	   0,	   1,	   0,	  0 },

	/* data format of "8 Bit" and "RGB565" */

	/* lcmBusW lcmMode lcmCyc pnlRBChg pnlGChg pktMode bigEnd midEnd */
	{  0,	   0,	   2,	  0,	   0,	   0,	   0,	  0 },
	{  0,	   0,	   2,	  0,	   1,	   1,	   0,	  0 },
	{  0,	   0,	   2,	  1,	   1,	   1,	   0,	  0 },
	{  0,	   0,	   2,	  0,	   0,	   1,	   0,	  0 },
	{  0,	   0,	   2,	  1,	   0,	   1,	   0,	  0 },

	/* data format of "8 Bit" and "RGB666" */

	/* lcmBusW lcmMode lcmCyc pnlRBChg pnlGChg pktMode bigEnd midEnd */
	{  0,	   1,	   3,	  0,	   0,	   0,	   0,	  0 },
	{  0,	   1,	   3,	  0,	   0,	   0,	   1,	  0 },

	/* data format of "9 Bit" and "RGB666" */

	/* lcmBusW lcmMode lcmCyc pnlRBChg pnlGChg pktMode bigEnd midEnd */
	{  1,	   0,	   2,	  0,	   1,	   1,	   0,	  0 },
	{  1,	   0,	   2,	  0,	   0,	   1,	   0,	  0 },
	{  1,	   0,	   2,	  0,	   0,	   0,	   0,	  0 },

	/* data format of "16 Bit" and "RGB444" */

	/* lcmBusW lcmMode lcmCyc pnlRBChg pnlGChg pktMode bigEnd midEnd */
	{  2,	   2,	   1,	  0,	   0,	   0,	   0,	  0 },
	{  2,	   2,	   1,	  1,	   0,	   0,	   0,	  0 },
	{  2,	   2,	   1,	  0,	   0,	   0,	   1,	  0 },
	{  2,	   2,	   1,	  1,	   0,	   0,	   1,	  0 },

	/* data format of "16 Bit" and "RGB565" */

	/* lcmBusW lcmMode lcmCyc pnlRBChg pnlGChg pktMode bigEnd midEnd */
	{  2,	   0,	   1,	  1,	   0,	   0,	   0,	  0 },
	{  2,	   0,	   1,	  0,	   0,	   0,	   0,	  0 },

	/* data format of "16 Bit" and "RGB666" */

	/* lcmBusW lcmMode lcmCyc pnlRBChg pnlGChg pktMode bigEnd midEnd */
	{  2,	   1,	   2,	  0,	   0,	   1,	   0,	  0 },
	{  2,	   1,	   2,	  0,	   0,	   0,	   1,	  0 },
	{  2,	   1,	   2,	  0,	   0,	   0,	   0,	  1 },
	{  2,	   1,	   2,	  0,	   0,	   0,	   0,	  0 },

	{  2,	   1,	   3,	  0,	   0,	   0,	   0,	  0 },
	{  2,	   1,	   3,	  0,	   0,	   1,	   0,	  0 },
	{  2,	   1,	   3,	  0,	   0,	   0,	   1,	  0 },
	{  2,	   1,	   3,	  0,	   0,	   1,	   1,	  0 },
	{  2,	   1,	   3,	  0,	   0,	   0,	   0,	  1 },
	{  2,	   1,	   3,	  0,	   0,	   1,	   0,	  1 },
	{  2,	   1,	   3,	  0,	   0,	   0,	   1,	  1 },
	{  2,	   1,	   3,	  0,	   0,	   1,	   1,	  1 },

	/* data format of "18 Bit" and "RGB666" */

	/* lcmBusW lcmMode lcmCyc pnlRBChg pnlGChg pktMode bigEnd midEnd */
	{  3,	   0,	   1,	  1,	   0,	   0,	   0,	  0 },
	{  3,	   0,	   1,	  0,	   0,	   0,	   0,	  0 },

	/* data format of "24 Bit" and "RGB666" */

	/* lcmBusW lcmMode lcmCyc pnlRBChg pnlGChg pktMode bigEnd midEnd */
	{  4,	   0,	   1,	  1,	   0,	   0,	   0,	  0 },
	{  4,	   0,	   1,	  0,	   0,	   0,	   0,	  0 },
};

/**************************************************************************
*                 E X T E R N A L    R E F E R E N C E S                 *
**************************************************************************/

/**************************************************************************
*               F U N C T I O N    D E C L A R A T I O N S               *
**************************************************************************/

/**
 * @fn        void HAL_DispParaSwRst(SINT16 *paraAry)
 * @brief     Reset display module
 * @param     paraAry[0] = [in] reset device, one of dispRstDev_e.
 * @param     paraAry[1] = [in] N/A, fill 0.
 * @param     paraAry[2] = [in] N/A, fill 0.
 * @param     paraAry[3] = [in] N/A, fill 0.
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaSwRst(
	SINT16 *paraAry
)
{
	XBYTE[REG_Disp_SwRst] |= paraAry[0];
	XBYTE[REG_Disp_SwRst] &= ~0x10; //panel clock enable
	XBYTE[REG_Disp_SwRst] &= ~paraAry[0];
	XBYTE[REG_Disp_SwRst] |= 0x10; //panel clock enable
}

/**
 * @fn        void HAL_DispParaClkEnableSet(SINT16 *paraAry)
 * @brief     enable/disable display clk
 * @param     paraAry[0] = [in] 0: disable, 1: enable
 * @param     paraAry[1] = [in] N/A, fill 0
 * @param     paraAry[2] = [in] N/A, fill 0
 * @param     paraAry[3] = [in] N/A, fill 0
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaClkEnableSet(
	SINT16 *paraAry
)
{
	if ( paraAry[0] ) {
		/* enable display nx clk & 1x clk. */
		XBYTE[0x2013] |= 0x10;

		/* enable display mclk. */
		XBYTE[0x2015] |= 0x08;
	}
	else {
		/* disable display nx clk & 1x clk. */
		XBYTE[0x2013] &= ~0x10;

		/* disable display mclk. */
		XBYTE[0x2015] &= ~0x08;
	}
}

/**
 * @fn        void HAL_DispParaClkCfgSet(SINT16 *paraAry)
 * @brief     set display clk configuration
 * @param     paraAry[0] = [in] clk source, one of HAL_DispClkSrc_e
 * @param     paraAry[1] = [in] N/A, fill 0
 * @param     paraAry[2] = [in] N/A, fill 0
 * @param     paraAry[3] = [in] N/A, fill 0
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaClkCfgSet(
	SINT16 *paraAry
)
{
	/* set dispclksel. */
	XBYTE[0x2019] = (XBYTE[0x2019] & ~0x01) | paraAry[0];

	/* set dispnxdiv. */
	if ( paraAry[1] >= 256 ) {
		paraAry[1] = 255;
	}
	else if ( paraAry[1] > 0 ) {
		paraAry[1]--;
	}
	XBYTE[0x2028] = paraAry[1];

	/* set disp1xdiv. */
	if ( paraAry[2] >= 4 ) {
		paraAry[2] = 3;
	}
	else if ( paraAry[2] > 0 ) {
		paraAry[2]--;
	}
	XBYTE[0x2029] = paraAry[2];
}

/**
 * @fn        void HAL_DispParaClkPhaseDlyCfg(SINT16 *paraAry)
 * @brief     configure the display clock phase
 * @param     paraAry[0] = [in] one of the HAL_DispClkPhaseDly_e
 * @param     paraAry[1] = [in] N/A, fill 0
 * @param     paraAry[2] = [in] N/A, fill 0
 * @param     paraAry[3] = [in] N/A, fill 0
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-9-8
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaClkPhaseDlyCfg(
	SINT16 *paraAry
)
{
	XBYTE[REG_Disp_PhsSel_Polar] = (XBYTE[REG_Disp_PhsSel_Polar] & ~0x07) | (paraAry[0] & 0x07);
}

/**
 * @fn        void HAL_DispParaSignalPolarCfg(SINT16 *paraAry)
 * @brief     set the display clock / VSYNC / HSYNC polarity
 * @param     paraAry[0] = [in] one of the HAL_DispSignalPolar_e
 * @param     paraAry[1] = [in] N/A, fill 0
 * @param     paraAry[2] = [in] N/A, fill 0
 * @param     paraAry[3] = [in] N/A, fill 0
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-9-8
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaSignalPolarCfg(
	SINT16 *paraAry
)
{
	XBYTE[REG_Disp_PhsSel_Polar] = (XBYTE[REG_Disp_PhsSel_Polar] & ~0x38) | (paraAry[0] & 0x38);
}

/**
 * @fn        void HAL_DispParaGlobEnableSet(SINT16 *paraAry)
 * @brief     enable/disable display controller
 * @param     paraAry[0] = [in] 0: disable, 1: enable
 * @param     paraAry[1] = [in] N/A, fill 0
 * @param     paraAry[2] = [in] N/A, fill 0
 * @param     paraAry[3] = [in] N/A, fill 0
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaGlobEnableSet(
 SINT16 *paraAry
)
{
	/* set dispen. */
	XBYTE[REG_Disp_DispEn_Flip] = (XBYTE[REG_Disp_DispEn_Flip] & ~0x01) | ((paraAry[0]?1:0) << 0);
	if (paraAry[0]) {
		XBYTE[REG_Dram_LensGainUpdt] |= 0x04; //enable Mpgsb flow control for display
	} else {
		XBYTE[REG_Dram_LensGainUpdt] &= ~0x04; //disable Mpgsb flow control for display
	}
}

/**
 * @fn        void HAL_DispParaImgLayerEnableSet(SINT16 *paraAry)
 * @brief     enable/disable image layer
 * @param     paraAry[0] = [in] 0: disable, 1: enable
 * @param     paraAry[1] = [in] N/A, fill 0
 * @param     paraAry[2] = [in] N/A, fill 0
 * @param     paraAry[3] = [in] N/A, fill 0
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaImgLayerEnableSet(
	SINT16 *paraAry
)
{
	XBYTE[REG_Disp_DispEn_Flip] = (XBYTE[REG_Disp_DispEn_Flip] & ~0x02) | ((paraAry[0]?1:0) << 1);
}

/**
 * @fn        void HAL_DispParaImgLayerFlipSet(SINT16 *paraAry)
 * @brief     set horizontal and vertical flip of image layer
 * @param     paraAry[0] = [in] bitmap [1:0] flip value, bit 0: hflip, bit 1: vflip
 * @param     paraAry[1] = [in] bitmap [1:0] mask, bit 0: hflip, bit 1: vflip
 * @param     paraAry[2] = [in] N/A, fill 0
 * @param     paraAry[3] = [in] N/A, fill 0
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaImgLayerFlipSet(
	SINT16 *paraAry
)
{
	/* set hflip & vflip. */
	paraAry[0] <<= 4;	/* bit 4: hflip, bit 3: vflip. */
	paraAry[1] <<= 4;	/* bit 4: hflip, bit 3: vflip. */
	XBYTE[REG_Disp_DispEn_Flip] = (XBYTE[REG_Disp_DispEn_Flip] & ~paraAry[1]) | (paraAry[0] & paraAry[1]);
}

/**
 * @fn        void HAL_DispParaImgLayerMonochromeEnableSet(SINT16 *paraAry)
 * @brief     enable/disable image layer monochrome (b&w)
 * @param     paraAry[0] = [in] 0: disable, 1: enable
 * @param     paraAry[1] = [in] N/A, fill 0
 * @param     paraAry[2] = [in] N/A, fill 0
 * @param     paraAry[3] = [in] N/A, fill 0
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaImgLayerMonochromeEnableSet(
	SINT16 *paraAry
)
{
	/* set bwen. */
	XBYTE[REG_Disp_ExtendMode] = (XBYTE[REG_Disp_ExtendMode] & ~0x02) | ((paraAry[0]?1:0) << 1);
}

/**
 * @fn        void HAL_DispParaOsdLayerEnableSet(SINT16 *paraAry)
 * @brief     enable/disable osd layer
 * @param     paraAry[0] = [in] 0: disable, 1: enable
 * @param     paraAry[1] = [in] N/A, fill 0
 * @param     paraAry[2] = [in] N/A, fill 0
 * @param     paraAry[3] = [in] N/A, fill 0
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaOsdLayerEnableSet(
	SINT16 *paraAry
)
{
	/* set glbosden. */
	XBYTE[REG_Disp_DispEn_Flip] = (XBYTE[REG_Disp_DispEn_Flip] & ~0x04) | ((paraAry[0]?1:0) << 2);
}

/**
 * @fn        void HAL_DispParaBgColorSet(SINT16 *paraAry)
 * @brief     set background color
 * @param     paraAry[0] = [in] r value, 0~255
 * @param     paraAry[1] = [in] g value, 0~255
 * @param     paraAry[2] = [in] b value, 0~255
 * @param     paraAry[3] = [in] N/A, fill 0
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaBgColorSet(
	SINT16 *paraAry
)
{
	/* set colorbgr. */
	XBYTE[REG_Disp_BgColorR] = paraAry[0];

	/* set colorbgg. */
	XBYTE[REG_Disp_BgColorG] = paraAry[1];

	/* set colorbgb. */
	XBYTE[REG_Disp_BgColorB] = paraAry[2];
}

/**
 * @fn        void HAL_DispParaReqLenthSet(SINT16 *paraAry)
 * @brief     set the image / OSD request burst length
 * @param     paraAry[0] = [in] one of the HAL_DispReqLenth_e
 * @param     paraAry[1] = [in] N/A, fill 0
 * @param     paraAry[2] = [in] N/A, fill 0
 * @param     paraAry[3] = [in] N/A, fill 0
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-9-8
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaReqLenthSet(
	SINT16 *paraAry
)
{
	XBYTE[REG_Disp_ImgOsdReq] = (XBYTE[REG_Disp_ImgOsdReq] & ~0x30) | (paraAry[0] & 0x30);
}
/**
 * @fn        void HAL_DispParaDeviceTypeSet(SINT16 *paraAry)
 * @brief     set display device type
 * @param     paraAry[0] = [in] device type, one of HAL_DispDeviceType_e
 * @param     paraAry[1] = [in] N/A, fill 0
 * @param     paraAry[2] = [in] N/A, fill 0
 * @param     paraAry[3] = [in] N/A, fill 0
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaDeviceTypeSet(
	SINT16 *paraAry
)
{
	/* set disp_mode. */
	XBYTE[REG_Disp_PnlSel] = (XBYTE[REG_Disp_PnlSel] & ~0x1F) | paraAry[0];
}

/**
 * @fn        void HAL_DispParaDeviceTotalRegionSet(SINT16 *paraAry)
 * @brief     set display total region
 * @param     paraAry[0] = [in] hpixel, total pixels per line
 * @param     paraAry[1] = [in] vline, total lines per frame
 * @param     paraAry[2] = [in] hsyncw, hsync rising time point
 * @param     paraAry[3] = [in] vsyncw, vsync rising time point
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaDeviceTotalRegionSet(
	SINT16 *paraAry
)
{
	/* set hpixel. */
	XBYTE[REG_Disp_HpixelL] = READ8(paraAry[0], 1);
	XBYTE[REG_Disp_HpixelH] = READ8(paraAry[0], 0);

	/* set vline. */
	XBYTE[REG_Disp_VlineL] = READ8(paraAry[1], 1);
	XBYTE[REG_Disp_VlineH] = READ8(paraAry[1], 0);

	/* set hsyncw. */
	XBYTE[REG_Disp_Hsyncw] = paraAry[2];

	/* set vsyncw. */
	XBYTE[REG_Disp_Vsyncw] = paraAry[3];
}

/**
 * @fn        void HAL_DispParaDeviceActiveRegionSet(SINT16 *paraAry)
 * @brief     set display active region
 * @param     paraAry[0] = [in] panel h start
 * @param     paraAry[1] = [in] panel v start
 * @param     paraAry[2] = [in] panel h end
 * @param     paraAry[3] = [in] panel v end
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaDeviceActiveRegionSet(
	SINT16 *paraAry
)
{
	/* set vldx0 (pnlh0). */
	XBYTE[REG_Disp_PnlH0L] = READ8(paraAry[0], 1);
	XBYTE[REG_Disp_PnlH0H] = READ8(paraAry[0], 0);

	/* set vldy0 (pnlv0). */
	XBYTE[REG_Disp_PnlV0L] = READ8(paraAry[1], 1);
	XBYTE[REG_Disp_PnlV0H] = READ8(paraAry[1], 0);

	/* set vldx1 (pnlh1). */
	XBYTE[REG_Disp_PnlH1L] = READ8(paraAry[2], 1);
	XBYTE[REG_Disp_PnlH1H] = READ8(paraAry[2], 0);

	/* set vldy1 (pnlv1). */
	XBYTE[REG_Disp_PnlV1L] = READ8(paraAry[3], 1);
	XBYTE[REG_Disp_PnlV1H] = READ8(paraAry[3], 0);
}

/**
 * @fn        void HAL_DispParaDeviceImgRegionCropSet(SINT16 *paraAry)
 * @brief     set display image region crop
 * @param     paraAry[0] = [in] region crop x start
 * @param     paraAry[1] = [in] region crop x end
 * @param     paraAry[2] = [in] region crop y start
 * @param     paraAry[3] = [in] region crop y end
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaDeviceImgRegionCropSet(
	SINT16 *paraAry
)
{
	/* set pvx0 */
	XBYTE[REG_Disp_PvX0L] = READ8(paraAry[0], 1);
	XBYTE[REG_Disp_PvX0H] = READ8(paraAry[0], 0);

	/* set pvx1 */
	XBYTE[REG_Disp_PvX1L] = READ8(paraAry[1], 1);
	XBYTE[REG_Disp_PvX1H] = READ8(paraAry[1], 0);

	/* set pvy0 */
	XBYTE[REG_Disp_PvY0L] = READ8(paraAry[2], 1);
	XBYTE[REG_Disp_PvY0H] = READ8(paraAry[2], 0);

	/* set pvy1 */
	XBYTE[REG_Disp_PvY1L] = READ8(paraAry[3], 1);
	XBYTE[REG_Disp_PvY1H] = READ8(paraAry[3], 0);
}

/**
 * @fn        void HAL_DispParaDeviceImgRegionRenderSet(SINT16 *paraAry)
 * @brief     set display image region render
 * @param     paraAry[0] = [in] region render h start
 * @param     paraAry[1] = [in] region render h end
 * @param     paraAry[2] = [in] region render v start
 * @param     paraAry[3] = [in] region render v end
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaDeviceImgRegionRenderSet(
	SINT16 *paraAry
)
{
	HAL_DispPnlInitHvSet(paraAry[0], paraAry[1], paraAry[2], paraAry[3]);

	/* set pvh0 */
	XBYTE[REG_Disp_PvH0L] = READ8(paraAry[0], 1);
	XBYTE[REG_Disp_PvH0H] = READ8(paraAry[0], 0);

	/* set pvh1 */
	XBYTE[REG_Disp_PvH1L] = READ8(paraAry[1], 1);
	XBYTE[REG_Disp_PvH1H] = READ8(paraAry[1], 0);

	/* set pvv0 */
	XBYTE[REG_Disp_PvV0L] = READ8(paraAry[2], 1);
	XBYTE[REG_Disp_PvV0H] = READ8(paraAry[2], 0);

	/* set pvv1 */
	XBYTE[REG_Disp_PvV1L] = READ8(paraAry[3], 1);
	XBYTE[REG_Disp_PvV1H] = READ8(paraAry[3], 0);
}

/**
 * @fn        void HAL_DispParaDeviceOsdRegionCropSet(SINT16 *paraAry)
 * @brief     set display osd region crop
 * @param     paraAry[0] = [in] region crop x start
 * @param     paraAry[1] = [in] region crop x end
 * @param     paraAry[2] = [in] region crop y start
 * @param     paraAry[3] = [in] region crop y end
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaDeviceOsdRegionCropSet(
	SINT16 *paraAry
)
{
	/* set osdx0 */
	XBYTE[REG_Disp_OsdX0L] = READ8(paraAry[0], 1);
	XBYTE[REG_Disp_OsdX0H] = READ8(paraAry[0], 0);

	/* set osdx1 */
	XBYTE[REG_Disp_OsdX1L] = READ8(paraAry[1], 1);
	XBYTE[REG_Disp_OsdX1H] = READ8(paraAry[1], 0);

	/* set osdy0 */
	XBYTE[REG_Disp_OsdY0L] = READ8(paraAry[2], 1);
	XBYTE[REG_Disp_OsdY0H] = READ8(paraAry[2], 0);

	/* set osdy1 */
	XBYTE[REG_Disp_OsdY1L] = READ8(paraAry[3], 1);
	XBYTE[REG_Disp_OsdY1H] = READ8(paraAry[3], 0);
}

/**
 * @fn        void HAL_DispParaDeviceOsdRegionRenderSet(SINT16 *paraAry)
 * @brief     set display osd region render
 * @param     paraAry[0] = [in] region render h start
 * @param     paraAry[1] = [in] region render h end
 * @param     paraAry[2] = [in] region render v start
 * @param     paraAry[3] = [in] region render v end
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaDeviceOsdRegionRenderSet(
	SINT16 *paraAry
)
{
	/* set osdh0 */
	XBYTE[REG_Disp_OsdH0L] = READ8(paraAry[0], 1);
	XBYTE[REG_Disp_OsdH0H] = READ8(paraAry[0], 0);

	/* set osdh1 */
	XBYTE[REG_Disp_OsdH1L] = READ8(paraAry[1], 1);
	XBYTE[REG_Disp_OsdH1H] = READ8(paraAry[1], 0);

	/* set osdv0 */
	XBYTE[REG_Disp_OsdV0L] = READ8(paraAry[2], 1);
	XBYTE[REG_Disp_OsdV0H] = READ8(paraAry[2], 0);

	/* set osdv1 */
	XBYTE[REG_Disp_OsdV1L] = READ8(paraAry[3], 1);
	XBYTE[REG_Disp_OsdV1H] = READ8(paraAry[3], 0);
}

/**
 * @fn        void HAL_DispParaOsdBlendFactorSet(SINT16 *paraAry)
 * @brief     set osd blending (half-tone) factor
 * @param     paraAry[0] = [in] 0~3, blending color index
 * @param     paraAry[1] = [in] 0~0x3f blending value
 * @param     paraAry[2] = [in] N/A, fill 0
 * @param     paraAry[3] = [in] N/A, fill 0
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaOsdBlendFactorSet(
	SINT16 *paraAry
)
{
	/* set htonefac. */
	if (paraAry[0] < 4) {
		XBYTE[REG_Disp_OsdAlpha0 + paraAry[0]] = paraAry[1];
	}
}

/**
* @brief	 enable/disable dither
* @param	 paraAry[0] = [in] one of HAL_DispInterlaceCfg_e.
* @param	 paraAry[1] = [in] N/A, fill 0.
* @param	 paraAry[2] = [in] N/A, fill 0.
* @param	 paraAry[3] = [in] N/A, fill 0.
* @retval  None.
* @see
* @author  Matt Wang
* @since	 2008-06-03
* @todo	 N/A
* @bug	 N/A
*/
void
HAL_DispParaInterlaceCfgSet(
	SINT16 *paraAry
)
{
	/* set fldoff & ofsten. */
	XBYTE[REG_Disp_FieldEnSwap] = (XBYTE[REG_Disp_FieldEnSwap] & ~0x05) | paraAry[0];
}

/**
* @brief	 enable/disable lcd dither
* @param	 paraAry[0] = [in] 0: disable, 1: enable.
* @param	 paraAry[1] = [in] N/A, fill 0.
* @param	 paraAry[2] = [in] N/A, fill 0.
* @param	 paraAry[3] = [in] N/A, fill 0.
* @retval  None.
* @see
* @author  Matt Wang
* @since	 2008-05-28
* @todo	 N/A
* @bug	 N/A
*/
void
HAL_DispParaLcdDitherEnableSet(
	SINT16 *paraAry
)
{
	paraAry = paraAry;

#if TAG_TODO
	/* TODO */

	/* set dien. */
	XBYTE[REG_Disp_OsdX1H] = (XBYTE[REG_Disp_OsdX1H] & ~0x02) | (paraAry[0] << 1);
#endif
}

/**
 * @fn        void HAL_DispParaUps051PixelShiftCfgSet(SINT16 *paraAry)
 * @brief     set ups051 pixel shift
 * @param     paraAry[0] = [in] one of HAL_DispUps051PixelShiftCfg_e
 * @param     paraAry[1] = [in] N/A, fill 0
 * @param     paraAry[2] = [in] N/A, fill 0
 * @param     paraAry[3] = [in] N/A, fill 0
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaUps051PixelShiftCfgSet(
	SINT16 *paraAry
)
{
	/* set pixshiften & vcntinv. */
	XBYTE[REG_Disp_LvEn_Rgb888] = (XBYTE[REG_Disp_LvEn_Rgb888] & ~0x30) | paraAry[0];
}

/**
 * @fn        void HAL_DispParaUps051LightviewEnableSet(SINT16 *paraAry)
 * @brief     enable/disable ups051 lightview
 * @param     paraAry[0] = [in] 0: disable, 1: enable
 * @param     paraAry[1] = [in] N/A, fill 0
 * @param     paraAry[2] = [in] N/A, fill 0
 * @param     paraAry[3] = [in] N/A, fill 0
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaUps051LightviewEnableSet(
	SINT16 *paraAry
)
{
	/* set lven. */
	XBYTE[REG_Disp_LvEn_Rgb888] = (XBYTE[REG_Disp_LvEn_Rgb888] & ~0x01) | (paraAry[0] << 0);
}

/**
 * @fn        void HAL_DispParaUps051RGB888EnableSet(SINT16 *paraAry)
 * @brief     enable / disable parallel RGB888 mode and RGB888 DDR mode
 * @param     paraAry[0] = [in] 0: disable, 1: enable for parallel RGB888 mode
 * @param     paraAry[1] = [in] 0: disable, 1: enable for RGB888 DDR mode
 * @param     paraAry[2] = [in] N/A, fill 0
 * @param     paraAry[3] = [in] N/A, fill 0
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-9-8
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaUps051RGB888EnableSet(
	SINT16 *paraAry
)
{
	/* set RGB888 */
	XBYTE[REG_Disp_LvEn_Rgb888] = (XBYTE[REG_Disp_LvEn_Rgb888] & ~0x06) | \
					(paraAry[0] ? (0x01 << 1) : 0x00) | \
					(paraAry[1] ? (0x01 << 2) : 0x00);
}

/**
 * @fn        void HAL_DispParaUps052RgbOnlyEnableSet(SINT16 *paraAry)
 * @brief     enable/disable ups052 rgb only mode
 * @param     paraAry[0] = [in] 0: disable, 1: enable
 * @param     paraAry[1] = [in] N/A, fill 0
 * @param     paraAry[2] = [in] N/A, fill 0
 * @param     paraAry[3] = [in] N/A, fill 0
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaUps052RgbOnlyEnableSet(
	SINT16 *paraAry
)
{
	/* set ups052rgbonly. */
	XBYTE[REG_Disp_UpsRgbOnly] = (XBYTE[REG_Disp_UpsRgbOnly] & ~0x01) | (paraAry[0] << 0);
}

/**
 * @fn        HAL_DispUps052HsdDelayCfg_e HAL_DispParaUpsHsdDelayCfgSet(SINT16 *paraAry)
 * @brief     set ups051/052 hsd config
 * @param     paraAry[0] = [in] one of HAL_DispUps052HsdDelayCfg_e or one of HAL_DispUps051HsdDelayCfg_e
 * @param     paraAry[1] = [in] N/A, fill 0
 * @param     paraAry[2] = [in] N/A, fill 0
 * @param     paraAry[3] = [in] N/A, fill 0
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaUpsHsdDelayCfgSet(
	SINT16 *paraAry
)
{
	/* set ups052rgbonly. */
	XBYTE[REG_Disp_UpshdSel] = (XBYTE[REG_Disp_UpshdSel] & ~0x07) | paraAry[0];
}

/**
 * @fn        void HAL_DispParaUpsClkPhaseSet(SINT16 *paraAry)
 * @brief     set ups clock phase (delay)
 * @param     paraAry[0] = [in] clock phase, 0~7
 * @param     paraAry[1] = [in] 0: normal 1: inverse
 * @param     paraAry[2] = [in] N/A, fill 0
 * @param     paraAry[3] = [in] N/A, fill 0
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaUpsClkPhaseSet(
	SINT16 *paraAry
)
{
	/* set ups_ck_phase. */
	XBYTE[REG_Disp_PhsSel_Polar] = (XBYTE[REG_Disp_PhsSel_Polar] & ~0x0f) | paraAry[0] | paraAry[1] << 3;
}

/**
 * @fn        void HAL_DispParaUpsLayoutModeSet(SINT16 *paraAry)
 * @brief     set ups layout mode
 * @param     paraAry[0] = [in] one of HAL_DispUpsLayoutMode_e
 * @param     paraAry[1] = [in] N/A, fill 0
 * @param     paraAry[2] = [in] N/A, fill 0
 * @param     paraAry[3] = [in] N/A, fill 0
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaUpsLayoutModeSet(
	SINT16 *paraAry
)
{
	/* set layoutmode. */
	XBYTE[REG_Disp_LvEn_Rgb888] = (XBYTE[REG_Disp_LvEn_Rgb888] & ~0x30) | paraAry[0];
}

/**
 * @fn        void HAL_DispParaUpsColorFilterCfgSet(SINT16 *paraAry)
 * @brief     set ups color filter (rgb cycle)
 * @param     paraAry[0] = [in] one of HAL_DispUpsEvenColorFilter_e
 * @param     paraAry[1] = [in] one of HAL_DispUpsOddColorFilter_e
 * @param     paraAry[2] = [in] N/A, fill 0
 * @param     paraAry[3] = [in] N/A, fill 0
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaUpsColorFilterCfgSet(
	SINT16 *paraAry
)
{
	/* set oddrgb & evenrgb. */
	XBYTE[REG_Disp_RgbEvenOdd] = paraAry[0] | paraAry[1];
}

/**
 * @fn        void HAL_DispParaCcirOutputSequenceCfgSet(SINT16 *paraAry)
 * @brief     set ccir output sequence
 * @param     paraAry[0] = [in] one of HAL_DispCcirOutputSeqCfg_e
 * @param     paraAry[1] = [in] N/A, fill 0
 * @param     paraAry[2] = [in] N/A, fill 0
 * @param     paraAry[3] = [in] N/A, fill 0
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaCcirOutputSequenceCfgSet(
	SINT16 *paraAry
)
{
	/* set ccirtype. */
	XBYTE[REG_Disp_CcirOSeq] = paraAry[0];
}

/**
 * @fn        void HAL_DispRaraEdgeTypeSet(SINT16* paraAry)
 * @brief     set the disp edge type
 * @param     paraAry[0] = [in] one of HAL_DispEdgeType_e
 * @param     paraAry[1] = [in] N/A, fill 0
 * @param     paraAry[2] = [in] N/A, fill 0
 * @param     paraAry[3] = [in] N/A, fill 0
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-9-8
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispRaraEdgeTypeSet(
	SINT16* paraAry
)
{
	XBYTE[REG_Disp_EdgeTypeGain] = (XBYTE[REG_Disp_EdgeTypeGain] & ~0x03) | (paraAry[0] & 0x03);
}

/**
 * @fn        void HAL_DispRaraEdgeGainSet(SINT16* paraAry)
 * @brief     set the disp edge Gain
 * @param     paraAry[0] = [in] edge enhancement sharpness gain (1.3 unsigned)
 * @param     paraAry[1] = [in] N/A, fill 0
 * @param     paraAry[2] = [in] N/A, fill 0
 * @param     paraAry[3] = [in] N/A, fill 0
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-9-8
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispRaraEdgeGainSet(
	SINT16* paraAry
)
{
	XBYTE[REG_Disp_EdgeTypeGain] = (XBYTE[REG_Disp_EdgeTypeGain] & ~0xF0) | (paraAry[0] << 4);
}

/**
 * @fn        void HAL_DispParaColorMatrixTblCfgSet(SINT16 *paraAry)
 * @brief     set the display color matrix
 * @param     paraAry[0] = [in] Matrix A
 * @param     paraAry[1] = [in] Matrix B
 * @param     paraAry[2] = [in] N/A, fill 0
 * @param     paraAry[3] = [in] N/A, fill 0
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaColorMatrixTblCfgSet(
	SINT16 *paraAry
)
{
	UINT16 xdata* pMtxA = (UINT16*)paraAry[0];
	UINT16 xdata* pMtxB = (UINT16*)paraAry[1];
	UINT16 mtxVal;
	UINT8 i8, valShift;

	for (i8 = 0; i8 < 9; i8++) {
		mtxVal = pMtxA[i8];
		//printf("a mtxVal: %lx\n", (UINT32)mtxVal);
		/* read data */
		XBYTE[REG_Disp_ColA00L + i8] = READ8(mtxVal, 1); //format S2.8

		/* calculate the bit shift for MSB */
		if (i8 & 0x01) {
			valShift = 4;
		} else {
			valShift = 0;
		}

		XBYTE[REG_Disp_ColA00H01H + (i8 >> 1)] = (XBYTE[REG_Disp_ColA00H01H + (i8 >> 1)] & ~(0x07 << valShift)) | \
		(READ8(mtxVal, 0) << valShift);
	}

	for (i8 = 0; i8 < 3; i8++) {
		mtxVal = pMtxB[i8];
		//printf("b mtxVal: %lx\n", (UINT32)mtxVal);
		XBYTE[REG_Disp_ColB00H + i8] = READ8(mtxVal, 0); //format S7.4

		/* calculate the bit shift for LSB */
		if (i8 & 0x01) {
			valShift = 4;
		} else {
			valShift = 0;
		}

		XBYTE[REG_Disp_ColB00L01L + (i8 >> 1)] = (XBYTE[REG_Disp_ColB00L01L + (i8 >> 1)] & ~(0x07 << valShift)) | \
		(READ8(mtxVal, 1) << valShift);
	}
}
/**
 * @fn        void HAL_DispParaColorMatrixEnableSet(SINT16 *paraAry)
 * @brief     set the display color matrix
 * @param     paraAry[0] = [in] 0: disable, 1: enable
 * @param     paraAry[1] = [in] N/A, fill 0
 * @param     paraAry[2] = [in] N/A, fill 0
 * @param     paraAry[3] = [in] N/A, fill 0
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaColorMatrixEnableSet(
	SINT16 *paraAry
)
{
	/* set ColorMatrixEnable. */
	XBYTE[REG_Disp_ColMtxEnFmt] = (XBYTE[REG_Disp_ColMtxEnFmt] & ~0x01) | (paraAry[0] ? 0x01 : 0x00);
}
/**
 * @fn        void HAL_DispParaColorMatrixFmtCfg(SINT16 *paraAry)
 * @brief     set the display color matrix
 * @param     paraAry[0] = [in] one of the HAL_DispColorMtxCvsFmt_e
 * @param     paraAry[1] = [in] N/A, fill 0
 * @param     paraAry[2] = [in] N/A, fill 0
 * @param     paraAry[3] = [in] N/A, fill 0
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaColorMatrixFmtCfg(
	SINT16 *paraAry
)
{
	/* set ColorMatrixEnable. */
	XBYTE[REG_Disp_ColMtxEnFmt] = (XBYTE[REG_Disp_ColMtxEnFmt] & ~0x30) | paraAry[0];
}
/**
 * @fn        void HAL_DispParaGammaEnableSet(SINT16 *paraAry)
 * @brief     enable/disable gamma
 * @param     paraAry[0] = [in] 0: disable, 1: enable
 * @param     paraAry[1] = [in] N/A, fill 0
 * @param     paraAry[2] = [in] N/A, fill 0
 * @param     paraAry[3] = [in] N/A, fill 0
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaGammaEnableSet(
	SINT16 *paraAry
)
{
	/* set gammaen. */
	XBYTE[REG_Disp_GammaEnSel] = (XBYTE[REG_Disp_GammaEnSel] & ~0x01) | (paraAry[0] ? 0x01 : 0x00);
}

/**
 * @fn        void HAL_DispParaRBSwitchSet(SINT16 *paraAry)
 * @brief     enable/disable R/B switch
 * @param     [in] paraAry
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-9-8
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaRBSwitchSet(
	SINT16 *paraAry
)
{
	/* set R/B switch */
	XBYTE[REG_Disp_PnlRound_RB] = (XBYTE[REG_Disp_GammaEnSel] & ~0x10) | (paraAry[0] ? 0x10 : 0x00);
}

/**
 * @fn        void HAL_DispParaTvDacPowerEnableSet(SINT16 *paraAry)
 * @brief     set analog tv saturation value
 * @param     paraAry[0] = [in] 0: disable, 1: enable
 * @param     paraAry[1] = [in] N/A, fill 0
 * @param     paraAry[2] = [in] N/A, fill 0
 * @param     paraAry[3] = [in] N/A, fill 0
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaTvDacPowerEnableSet(
	SINT16 *paraAry
)
{
	if ( paraAry[0] ) {  /* enable. */
		/* set DACPd & DACTest. */
		XBYTE[0x2001] &= ~0x03;

		/* enable clock phase */
		XBYTE[0x2016] |= 0x01;

		/* set yfilteren & uvfilteren. */
		XBYTE[REG_Disp_PalmEn_Filter] |= 0x30;
	}
	else {  /* disable. */
		/* set DACPd & DACTest. */
		XBYTE[0x2001] |= 0x03;

		/* disable clock phase */
		XBYTE[0x2016] &= ~0x01;

		/* set yfilteren & uvfilteren. */
		XBYTE[REG_Disp_PalmEn_Filter] &= ~0x30;
	}

	//XBYTE[0x2009] = 0x17;  /*vtesten reseten rsetop[3:0]*/
}

/**
 * @fn        void HAL_DispParaTvSaturationSet(SINT16 *paraAry)
 * @brief     set analog tv saturation value
 * @param     paraAry[0] = [in] saturation value, 0~255
 * @param     paraAry[1] = [in] N/A, fill 0
 * @param     paraAry[2] = [in] N/A, fill 0
 * @param     paraAry[3] = [in] N/A, fill 0
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaTvSaturationSet(
	SINT16 *paraAry
)
{
	/* set saturate. */
	XBYTE[REG_Disp_SatuRateL] = paraAry[0];
}

/**
 * @fn        void HAL_DispParaTvHueSet(SINT16 *paraAry)
 * @brief     set analog tv hue value
 * @param     paraAry[0] = [in] hue value, 0~255
 * @param     paraAry[1] = [in] N/A, fill 0
 * @param     paraAry[2] = [in] N/A, fill 0
 * @param     paraAry[3] = [in] N/A, fill 0
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaTvHueSet(
	SINT16 *paraAry
)
{
	/* set hueadjust. */
	XBYTE[REG_Disp_HueAdjust] = paraAry[0];
}

/**
 * @fn        void HAL_DispParaLcmCmdBusTypeSet(SINT16 *paraAry)
 * @brief     set lcm command bus type
 * @param     paraAry[0] = [in] bus type, one of HAL_DispLcmCommandBusType_e
 * @param     paraAry[1] = [in] N/A, fill 0
 * @param     paraAry[2] = [in] N/A, fill 0
 * @param     paraAry[3] = [in] N/A, fill 0
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaLcmCmdBusTypeSet(
	SINT16 *paraAry
)
{
	/* set m68ctl1. */
	XBYTE[REG_Disp_LcmMode_BusW] = (XBYTE[REG_Disp_LcmMode_BusW] & ~0x01) | paraAry[0];
}

/**
 * @fn        void HAL_DispParaLcmCmdBitWidthSet(SINT16 *paraAry)
 * @brief     set lcm command bit width
 * @param     paraAry[0] = [in] bit width, one of HAL_DispLcmCommandBitWidth_e
 * @param     paraAry[1] = [in] N/A, fill 0
 * @param     paraAry[2] = [in] N/A, fill 0
 * @param     paraAry[3] = [in] N/A, fill 0
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaLcmCmdBitWidthSet(
	SINT16 *paraAry
)
{
	/* set h16bit. */
	XBYTE[REG_Disp_LcmMode_BusW] = (XBYTE[REG_Disp_LcmMode_BusW] & ~0xF0) | paraAry[0];
}

/**
* @brief	 set lcm command pulse width.
* @param	 paraAry[0] = [in] active pulse width, 1~256.
* @param	 paraAry[1] = [in] recovery pulse width, 2~257.
* @param	 paraAry[2] = [in] N/A, fill 0.
* @param	 paraAry[3] = [in] N/A, fill 0.
* @retval  None.
* @see
* @author  Matt Wang
* @since	 2010-02-03
* @todo	 N/A
* @bug	 N/A
*/
void
HAL_DispParaLcmCmdPulseWidthSet(
	SINT16 *paraAry
)
{
	paraAry = paraAry;

#if TAG_TODO
	/* should move this function */

	/* set actpulse. */
	if ( paraAry[0] > 256 ) {
		paraAry[0] = 255;
	}
	else if ( paraAry[0] < 1 ) {
		paraAry[0] = 0;
	}
	else {
		paraAry[0]--;
	}
	XBYTE[REG_Disp_GpioRIntEn0] = paraAry[0];

	/* set rcvpulse. */
	if ( paraAry[1] > 257 ) {
		paraAry[1] = 255;
	}
	else if ( paraAry[0] < 2 ) {
		paraAry[1] = 0;
	}
	else {
		paraAry[1] -= 2;
	}
	XBYTE[REG_Disp_GpioRIntEn1] = paraAry[1];
#endif
}

/**
 * @fn        void HAL_DispParaLcmCmdActiveLevelSet(SINT16 *paraAry)
 * @brief     set lcm cs0 and a0 active level
 * @param     paraAry[0] = [in] 0: cs0 active low, 1: cs0 active high
 * @param     paraAry[1] = [in] 0: a0 command low and data high, 1: a0 command high and data low
 * @param     paraAry[2] = [in] N/A, fill 0
 * @param     paraAry[3] = [in] N/A, fill 0
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaLcmCmdActiveLevelSet(
	SINT16 *paraAry
)
{
	/* set cs0. */
	if ( paraAry[0] ) {  /* high active. */
		HAL_DispCs0Act   = (1 << 0);
		HAL_DispCs0Inact = (0 << 0);
	}
	else {  /* low active. */
		HAL_DispCs0Act   = (0 << 0);
		HAL_DispCs0Inact = (1 << 0);
	}

	/* set a0. */
	if ( paraAry[1] ) {  /* 0: data, 1: cmd. */
		HAL_DispA0Cmd	= (1 << 1);
		HAL_DispA0Data = (0 << 1);
	}
	else {  /* 0: cmd, 1: data. */
		HAL_DispA0Cmd	= (0 << 1);
		HAL_DispA0Data = (1 << 1);
	}
}

/**
 * @fn        void HAL_DispParaLcmDataFormatSet(SINT16 *paraAry)
 * @brief     set lcm data format
 * @param     paraAry[0] = [in] data format, one of HAL_DispLcmDataFormat_e
 * @param     paraAry[1] = [in] N/A, fill 0
 * @param     paraAry[2] = [in] N/A, fill 0
 * @param     paraAry[3] = [in] N/A, fill 0
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DispParaLcmDataFormatSet(
	SINT16 *paraAry
)
{
	UINT16 fmtIdx = (UINT16)paraAry[0];
	lcmSetItem_t* pLcmFmt;

	if (fmtIdx >= sizeof(sLcmSetItem) / sizeof(lcmSetItem_t)) {
		return;
	}
	pLcmFmt = &sLcmSetItem[fmtIdx];
#if 0
	printf("busW:%bx, mode:%bx, cyc:%bx, rbchg:%bx, gchg:%bx, pktmode:%bx, bigend:%bx, midend:%bx\n",
	pLcmFmt->busWidth, pLcmFmt->dataType, pLcmFmt->dataCyc, pLcmFmt->pnlRBChg,\
	pLcmFmt->pnlGChg, pLcmFmt->pktMode, pLcmFmt->bigEnd, pLcmFmt->midEnd);
#endif

	/* set bus width */
	XBYTE[REG_Disp_LcmMode_BusW] = (XBYTE[REG_Disp_LcmMode_BusW] & ~0xF0) | (pLcmFmt->busWidth << 4);

	/* set data cycle and type */
	XBYTE[REG_Disp_LcmDatCyc_Type] = pLcmFmt->dataCyc | (pLcmFmt->dataType << 4);

	/* set data mode */
	XBYTE[REG_Disp_LcmDatFmt] = (pLcmFmt->pnlGChg << 1) | (pLcmFmt->pktMode << 4) | \
	 (pLcmFmt->bigEnd << 5) | (pLcmFmt->midEnd << 6);

	/* set RB change */
	XBYTE[REG_Disp_PnlRound_RB] = (XBYTE[REG_Disp_PnlRound_RB] & ~0xF0) | (pLcmFmt->pnlRBChg << 4);

}


/**
* @brief	 enable/disable lcm rgb666 pack.
* @param	 paraAry[0] = [in] 0: disable, 1: enable.
* @retval  None.
* @see
* @author  Matt Wang
* @since	 2010-02-03
* @todo	 N/A
* @bug	 N/A
*/
void
HAL_DispParaLcmRgb666PackEnableSet(
	SINT16 *paraAry
)
{
	paraAry = paraAry;

#if TAG_TODO
	/* should remove this function */

	/* set rgb666. */
	//XBYTE[REG_Disp_GpioOutEn0] = paraAry[0]; //phil mark for 1628
#endif
}

/**
* @brief	 enable/disable lcm rgb666 format 10.
* @param	 paraAry[0] = [in] 0: disable, 1: enable.
* @retval  None.
* @see
* @author  Matt Wang
* @since	 2010-02-03
* @todo	 N/A
* @bug	 N/A
*/
void
HAL_DispParaLcmRgb666Format10EnableSet(
	SINT16 *paraAry
)
{
	paraAry = paraAry;

#if TAG_TODO
	/* should remove this function */

	/* set rgb666format10. */
	//XBYTE[REG_Disp_GpioOutEn3] = (XBYTE[REG_Disp_GpioOutEn3] & ~0x08) | (paraAry[0] << 3); //phil mark for 1628
#endif
}

/**
* @brief	 set lcm pixel data alignment.
* @param	 paraAry[0] = [in] pixel data alignment, one of HAL_DispLcmPixelAlign_e.
* @retval  None.
* @see
* @author  Matt Wang
* @since	 2010-02-03
* @todo	 N/A
* @bug	 N/A
*/
void
HAL_DispParaLcmPixelAlignSet(
 SINT16 *paraAry
)
{
	paraAry = paraAry;

#if TAG_TODO
	/* should remove this function */

	/* set bigending & midending. */
	//XBYTE[REG_Disp_GpioOutEn2] = paraAry[0]; //phil mark for 1628
#endif
}

/**
* @brief	 set lcm color package.
* @param	 paraAry[0] = [in] color package, one of HAL_DispLcmColorPackage_e.
* @retval  None.
* @see
* @author  Matt Wang
* @since	 2010-02-03
* @todo	 N/A
* @bug	 N/A
*/
void
HAL_DispParaLcmColorPackageSet(
 SINT16 *paraAry
)
{
	paraAry = paraAry;

#if TAG_TODO
	/* should remove this function */

	/* set packmode. */
	//XBYTE[REG_Disp_GpioOutEn3] = (XBYTE[REG_Disp_GpioOutEn3] & ~0x01) | paraAry[0]; //phil mark for 1628
#endif
}

/**
* @brief	 enable/disable lcm switch g-component bits.
* @param	 paraAry[0] = [in] 0: disable (g[7:0]), 1: enable (g[4:2],g[7:5],g[1:0]).
* @param	 paraAry[1] = [in] N/A, fill 0.
* @param	 paraAry[2] = [in] N/A, fill 0.
* @param	 paraAry[3] = [in] N/A, fill 0.
* @retval  None.
* @see
* @author  Matt Wang
* @since	 2010-02-03
* @todo	 N/A
* @bug	 N/A
*/
void
HAL_DispParaLcmSwitchGEnableSet(
 SINT16 *paraAry
)
{
	paraAry = paraAry;

#if TAG_TODO
	/* should remove this function */

	/* set swgin. */
	//XBYTE[REG_Disp_GpioOutEn3] = (XBYTE[REG_Disp_GpioOutEn3] & ~0x02) | (paraAry[0] << 1); //phil mark for 1628
#endif
}

/**
* @brief	 enable/disable lcm change r- and b-components.
* @param	 paraAry[0] = [in] 0: disable, 1: enable.
* @param	 paraAry[1] = [in] N/A, fill 0.
* @param	 paraAry[2] = [in] N/A, fill 0.
* @param	 paraAry[3] = [in] N/A, fill 0.
* @retval  None.
* @see
* @author  Matt Wang
* @since	 2010-02-03
* @todo	 N/A
* @bug	 N/A
*/
void
HAL_DispParaLcmChangeRbEnableSet(
 SINT16 *paraAry
)
{
	paraAry = paraAry;

#if TAG_TODO
	/* should remove this function */

	/* set rbchange. */
	//XBYTE[REG_Disp_GpioOutEn3] = (XBYTE[REG_Disp_GpioOutEn3] & ~0x04) | (paraAry[0] << 2); //phil mark for 1628
#endif
}

/**
* @brief	 enable/disable lcm 2/3 pixel data output.
* @param	 paraAry[0] = [in] 0: disable, 1: enable.
* @param	 paraAry[1] = [in] N/A, fill 0.
* @param	 paraAry[2] = [in] N/A, fill 0.
* @param	 paraAry[3] = [in] N/A, fill 0.
* @retval  None.
* @see
* @author  Matt Wang
* @since	 2010-02-03
* @todo	 N/A
* @bug	 N/A
*/
void
HAL_DispParaLcmOneHalfPixelEnableSet(
 SINT16 *paraAry
)
{
	paraAry = paraAry;

#if TAG_TODO
	/* should remove this function */

	/* set onehalfmode. */
	//XBYTE[REG_Disp_GpioInVal0] = (XBYTE[REG_Disp_GpioInVal0] & ~0x01) | paraAry[0]; //phil mark for 1628
#endif
}

/**
* @brief	 enable/disable lcm dither.
* @param	 paraAry[0] = [in] 0: disable, 1: enable.
* @param	 paraAry[1] = [in] N/A, fill 0.
* @param	 paraAry[2] = [in] N/A, fill 0.
* @param	 paraAry[3] = [in] N/A, fill 0.
* @retval  None.
* @see
* @author  Matt Wang
* @since	 2010-02-03
* @todo	 N/A
* @bug	 N/A
*/
void
HAL_DispParaLcmDitherEnableSet(
 SINT16 *paraAry
)
{
	paraAry = paraAry;

#if TAG_TODO
	/* should remove this function */

	/* set ditheren. */
	//XBYTE[REG_Disp_GpioOutVal0] = (XBYTE[REG_Disp_GpioOutVal0] & ~0x02) | (paraAry[0] << 1); //phil mark for 1628
#endif
}

