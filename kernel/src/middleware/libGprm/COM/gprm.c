/**************************************************************************
 *         Copyright(c) 2007 by Sunplus mMedia  Technology Co., Ltd.       *
 *                                                                         *
 *  This software is copyrighted by and is the property of Sunplus mMedia  *
 *  Technology Co., Ltd. All rights are reserved by Sunplus mMedia Techno- *
 *  logy Co., Ltd. This software may only be used in accordance with the   *
 *  corresponding license agreement. Any unauthorized use, duplication,    *
 *  distribution, or disclosure of this software is expressly forbidden.   *
 *                                                                         *
 *  This Copyright notice "M U S T" not be removed or modified without     *
 *  prior written consent of Sunplus mMedia Technology  Co., Ltd.          *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd. reserves the right to modify this  *
 *  software without notice.                                               *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd.                                    *
 *  19-1, Innovation First Road, Science-Based Industrial Park,            *
 *  Hsin-Chu, Taiwan, R.O.C.                                               *
 *                                                                         *
 **************************************************************************/
/**
 * @filename 	gprm.c
 * @brief 	The main file of gprm
 * @author 	Wen-Guo
 * @since 	2008-6-13
 * @date 	2008-6-13
 */
#include "general.h"
#include "reg_def.h"

#include "hal_gprm.h"
#include "hal_disp.h"
#include "osd_api.h"
#include "osd_buff.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define GPRM_PEN_DRAW_WAIT  1
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

#define GPRM_SRC_V_EXTRA 20

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static UINT16 gprmVScalingFactor;
static UINT16 gprmVInitScalingFactor;
static UINT16 gprmVscaleLastLines;
static UINT16 gprmVscaleLastCnt;

static UINT16 gprmVScaleSrcX;
static UINT16 gprmVScaleSrcY;
static UINT16 gprmVScaleDstX;
static UINT16 gprmVScaleDstY;
static UINT16 gprmVScaleSrcLines;
static UINT16 gprmVScaleDstLines;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**************************************************************************
 *               S T A T I C   L O C A L   F U N C T I O N                *
 **************************************************************************/
static void
gprmVScalingParamCal(
	UINT16 i,
	UINT16 *m,
	UINT16 *offset,
	UINT16 *initFactor,
	UINT16 *overlap
)
{
	*m = gprmVScaleDstLines * i + 1 - (UINT16)((UINT32)65536 / (UINT32)gprmVScalingFactor );

	*overlap = gprmVScaleDstLines * i - *m + 1;

	*offset = (UINT32)( *m - 1 ) * (UINT32)gprmVScalingFactor / (UINT32)65536;
	*initFactor = ( (UINT32)( *m - 1 ) * (UINT32)gprmVScalingFactor % (UINT32)65536 );

	while( *initFactor >= 0x8000 ) {
		*m = *m - 1; // adjust

		*overlap = gprmVScaleDstLines * i - *m + 1;

		*offset = (UINT32)( *m - 1 ) * (UINT32)gprmVScalingFactor / (UINT32)65536;
		*initFactor = ( (UINT32)( *m - 1 ) * (UINT32)gprmVScalingFactor % (UINT32)65536 );
	}

	return;
}

/**************************************************************************
 *                     G L O B A L   F U N C T I O N                      *
 **************************************************************************/
UINT8
gprmPartialVScaleInit(
	UINT16 srcX,
	UINT16 srcY,
	UINT16 dstX,
	UINT16 dstY,
	UINT16 lines
)
{
	UINT16 maxCnt;

	gprmVScaleSrcX = srcX;
	gprmVScaleSrcY = srcY;
	gprmVScaleDstX = dstX;
	gprmVScaleDstY = dstY;
	gprmVScaleDstLines = lines;

	gprmVScalingFactor = (UINT16)(((UINT32)( gprmVScaleSrcY - 1 )<<16)/(UINT32)( gprmVScaleDstY - 1 ));

	maxCnt = ( dstY + ( gprmVScaleDstLines - 1 ) ) / gprmVScaleDstLines;

	gprmVScaleSrcLines = (UINT16)( (UINT32)gprmVScaleDstLines * gprmVScaleSrcY / gprmVScaleDstY );

	gprmVscaleLastLines = dstY - ( maxCnt - 1 ) * gprmVScaleDstLines;

	gprmVscaleLastCnt = maxCnt - 1;

	return 0;
}

UINT8
gprmPartialVScale(
	UINT32 srcAddr,
	UINT32 dstAddr,
	UINT32 tempAddr,
	UINT16 i,
	UINT16 *overlap
)
{
	UINT16 m, initFactor, offset;
	UINT32 addr;

	if( gprmVScaleSrcX == gprmVScaleDstX && gprmVScaleSrcY == gprmVScaleDstY ) {
		addr = srcAddr + (UINT32)i * gprmVScaleSrcLines * gprmVScaleSrcX;

		HAL_GprmDramDma( addr << 1, dstAddr << 1, (UINT32)gprmVScaleSrcLines * gprmVScaleSrcX * 2, 1 );

		*overlap = 0;
	}
	else {
		if( i == 0 ) {
			HAL_GprmScaleVFactor( srcAddr, gprmVScaleSrcX, gprmVScaleSrcLines + GPRM_SRC_V_EXTRA, dstAddr, gprmVScaleDstX, gprmVScaleDstLines, tempAddr, gprmVScalingFactor, 0x8000, 1 );

			*overlap = 0;
		}
		else {
			gprmVScalingParamCal( i, &m, &offset, &initFactor, overlap );

			addr = srcAddr + (UINT32)offset * gprmVScaleSrcX;

			if( i != gprmVscaleLastCnt ) {
				HAL_GprmScaleVFactor( addr, gprmVScaleSrcX, gprmVScaleSrcLines + GPRM_SRC_V_EXTRA, dstAddr, gprmVScaleDstX, gprmVScaleDstLines + (UINT16)( *overlap ), tempAddr, gprmVScalingFactor, initFactor + 0x8000, 1 );
			}
			else {
				HAL_GprmScaleVFactor( addr, gprmVScaleSrcX, gprmVScaleSrcY - offset, dstAddr, gprmVScaleDstX, gprmVscaleLastLines + (UINT16)( *overlap ), tempAddr, gprmVScalingFactor, initFactor + 0x8000, 1 );
			}
		}
	}

	return 0;
}


/**
 * @fn        void gprmPenCfg(UINT8 penColor)
 * @brief     gprmPenCfg
 * @param     [in] penColor
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
void 
gprmPenCfg(
	UINT8 penColor
)
{
	UINT32 buffAddrWord;
	UINT16 buffSizeW, buffSizeH;
	
	//HAL_DispOsdBuffAddrGet(&buffAddrWord);
	buffAddrWord = osdDispBuffAddrGet(1);
	HAL_DispOsdBuffSizeGet(&buffSizeW, &buffSizeH);

	buffSizeW >>= 1; //osd 8bit format

	/* configure to osd format */
	HAL_GprmPenDrawCfg(
		1,
		penColor, 0, 0,
		buffAddrWord, buffSizeW, buffSizeH
	);
}

/**
 * @fn        UINT8 gprmPenRectDraw(UINT16 x, UINT16 y, UINT16 w, UINT16 h)
 * @brief     gprmPenRectDraw
 * @param     [in] x
 * @param     [in] y
 * @param     [in] w
 * @param     [in] h
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
gprmPenRectDraw(
	UINT16 x,
	UINT16 y,
	UINT16 w,
	UINT16 h
)
{
	x >>= 1; // x*8/16
	w >>= 1; // w*8/16

	if (!w) {
		return FAIL; //no zero size passing to GPRM
	}
	
	return HAL_GprmPenRectDraw(x, y, w, h, GPRM_PEN_DRAW_WAIT);
}

/**
 * @fn        void gprmBlackFill(UINT32 addrWord, UINT16 imgW, UINT16 imgH)
 * @brief     gprmBlackFill
 * @param     [in] addrWord
 * @param     [in] imgW
 * @param     [in] imgH
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-18
 * @todo      N/A
 * @bug       N/A
*/

void gprmBlackFill(UINT32 addrWord, UINT16 imgW, UINT16 imgH)
{
	osdIndexSave();
	
	/* Y:0x00, U:0x80, V:0x80 */
	HAL_GprmPenDrawCfg(
		0,//data format YUV
		0x00, 0x80, 0x80,
		addrWord, imgW, imgH
	);
	XBYTE[REG_Gprm_PstThd] = 0x00; /* reset the threshold */

	HAL_GprmPenRectDraw(0, 0, imgW, imgH, 1);
	osdIndexRestore();
}

