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
 * @file      osd_draw.c
 * @brief     osd draw related function
 * @author    Phil Lin
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#define __FILE __FILE_ID_OSD_DRAW__
#include "general.h"
#include "dbg_def.h"

#include "hal.h"
#include "hal_disp.h"
#include "hal_gprm.h"

#include "gprm.h"
#include "disp_api.h"
#include "osd_api.h"
#include "osd_mem.h"
#include "osd_buff.h"
#include "initio.h"
#include "hal_dram.h"
#include "dbg_mem.h" 

#if MODULE_OSD_OPT
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define	OSD_DRAW_DEBUG	0

#define OSD_CHAR_BYTE_IDX_GRP	0x08 //0x00 ~ 0x07 reserved for word char group

#define OSD_CHAR_BUFF_WIDTH    512
#define OSD_CHAR_BUFF_HEIGHT   128

#define OSD_ICON_BUFF_WIDTH    512
#define OSD_ICON_BUFF_HEIGHT   256
//default width and height of char
#define CHAR_WIDTH_PIXEL 16

#define CHAR_HEIGHT_PIXEL 32


 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct
{
    UINT32 font_offset;
    UINT16 font_w_pixel;
    UINT16 font_w_word;
	UINT16 font_h_pixel;
} font_struct; 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static osdPen_t xdata gOsdPen; 
static UINT8 idxBldColor;
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
void osdFontSizeGet(UINT16* width, UINT16* height); 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/**
 * @fn        void osdClear(UINT8 attr)
 * @brief     osdClear
 * @param     [in] idx
 * @param     [in] attr
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-24
 * @todo      N/A
 * @bug       N/A
*/
void
osdClear(
	UINT8 attr
)
{
	UINT16 buffW, buffH;
	UINT32 dispAddrWord;
	attr = attr;
	dispAddrWord = osdDispBuffAddrGet(1);
	osdDispBuffSizeGet(&buffW, &buffH);
	osdIndexSave();
	osdPenCfg(1,1);
	osdRectDraw(0, 0, buffW, buffH);
	osdIndexRestore();
}



/**
 * @fn        void osdIndexSet(UINT8 save)
 * @brief     osdIndexSet
 * @param     [in] save  1:save   0:restore
 * @retval    NONE
 * @see       NULL
 * @author    dongdong.wang
 * @since     2010-8-30
 * @todo      N/A
 * @bug       N/A
*/
void osdIndexSetRestore(UINT8 save)
{
	static UINT8 reg2f40,reg2f41,reg2f42,reg2f43;
	if(save)//save
	{
		HAL_GprmOsdIdxGet(&reg2f40,&reg2f41,&reg2f42,&reg2f43);
	}
	else // restore
	{
		HAL_GprmOsdIdxSet(reg2f40,reg2f41,reg2f42,reg2f43);
	}
	
	
}


/**
 * @fn        void osdClearChar(SINT16 x, SINT16 y, UINT16 w, UINT16 h)
 * @brief     osdClearChar
 * @param     [in] x
 * @param     [in] y
 * @param     [in] w
 * @param     [in] h
 * @retval    NONE
 * @see       NULL
 * @author    dongdong.wang
 * @since     2010-8-24
 * @todo      N/A
 * @bug       N/A
*/
void
osdClearChar(
	SINT16 x,
	SINT16 y,
	UINT16 w,
	UINT16 h
)
{
	UINT16 buffW, buffH;
	UINT32 dispAddrWord;

#if (CHAR_WIDTH_PIXEL == 16) && (CHAR_HEIGHT_PIXEL == 32)
	x <<= 4;
	y <<= 4;
	w <<= 4;
	h <<= 4;
#else
	x *= CHAR_WIDTH_PIXEL;
	y *= CHAR_HEIGHT_PIXEL;
	w *= CHAR_WIDTH_PIXEL;
	h *= CHAR_HEIGHT_PIXEL;
#endif	
	dispAddrWord = osdDispBuffAddrGet(1);
	osdDispBuffSizeGet(&buffW, &buffH);
	osdIndexSave();
	osdPenCfg(1,1);
	osdRectDraw(x, y, w, h);
	osdIndexRestore();

	
}
void 
osdClearCharExt(
	SINT16 x,
	SINT16 y,
	UINT16 w,
	UINT16 h
)	
{
	osdIndexSave();
	osdPenCfg(1,1);
	osdRectDraw(x, y, w, h);
	osdIndexRestore();
}

/**
 * @fn        UINT8 osdPenCfg(UINT16 color, UINT16 width)
 * @brief     osdPenCfg
 * @param     [in] color [7:6]attr [5:0]color index
 * @param     [in] width
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-24
 * @todo      N/A
 * @bug       N/A
*/
UINT8
osdPenCfg(
	UINT16 color,
	UINT16 width
)
{	
	gOsdPen.color = color;
	gOsdPen.width = width;
	
	gprmPenCfg(color);
	
	return SUCCESS;
}

/**
 * @fn        void osdPenGet(const osdPen_t** pPen)
 * @brief     osdPenGet
 * @param     [in] pPen
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-24
 * @todo      N/A
 * @bug       N/A
*/
void osdPenGet(
	const osdPen_t** pPen
)
{
	*pPen = &gOsdPen;
}


void
osdPageDrawUpdate(
	UINT16 idx[],
	UINT8 attr[]
)
{
	idx = idx;
	attr = attr;
}

/**
 * @brief	Show OSD str on (x,y)
 * @param	x   : coord of x-axis
 * @param	y   : coord of y-axis
 * @param	idx : 0~255
 * @param	attr: if attr==-1, skip chg attr
 					bit[2:0]: foreground color index
 					bit3: background color enable
					bit[5:4]
						0	Normal mode.
						1	Half-tone mode
						2	Exchange foreground/background color
						3	Blinking mode.
 * @retval	NONE
 * @author	Ted Chiu
 * @since	07/12/27
 */
 //#pragma OT(8, SPEED)
void
osdStrDraw(
	UINT8 x,
	UINT8 y,
	UINT8 *str,
	UINT8 attr
)
{

	UINT16 offset = (UINT16)x << 4;

	while ( *str ) {
		if (*str >= OSD_CHAR_BYTE_IDX_GRP)
			offset += osdCharDrawExt(offset, (UINT16)y << 5, *str++,attr);
		else
			offset += osdCharDrawExt(offset, (UINT16)y << 5, *((UINT16*)str)++,attr); //Joe: Need Test ???
	}
}

/**
 * @brief	Show OSD str on (x,y)
 * @param	x   : coord of x-axis
 * @param	y   : coord of y-axis
 * @param	idx : 0~255
 * @param	attr: if attr==-1, skip chg attr
 					bit[2:0]: foreground color index
 					bit3: background color enable
					bit[5:4]
						0	Normal mode.
						1	Half-tone mode
						2	Exchange foreground/background color
						3	Blinking mode.
 * @retval	NONE
 * @author	Ted Chiu
 * @since	07/12/27
 */
void
osdStrDrawExt(
	UINT16 x,
	UINT16 y,
	UINT8 *str,
	UINT8 attr
)
{
	while ( *str ) {
		if (*str >= OSD_CHAR_BYTE_IDX_GRP)
			x += osdCharDrawExt(x, y, *str++,attr);
		else
			x += osdCharDrawExt(x, y, *((UINT16*)str)++,attr); //Joe: Need Test ???
	}
}

/**
 * @brief	Show OSD str on (x,y)
 * @param	x   : coord of x-axis
 * @param	y   : coord of y-axis
 * @param	idx : 0~511
 * @param	attr: if attr==-1, skip chg attr
 					bit[2:0]: foreground color index
 					bit3: background color enable
					bit[5:4]
						0	Normal mode.
						1	Half-tone mode
						2	Exchange foreground/background color
						3	Blinking mode.
 * @retval	NONE
 * @author	Ted Chiu
 * @since	07/12/27
 */
void
osdStrDraw16(
	UINT8 x,
	UINT8 y,
	UINT16 *str16,
	UINT8 attr
)
{
	UINT16 offset = (UINT16)x << 4;
	while ( *str16 ) {
		offset += osdCharDrawExt(offset, (UINT16)y << 5, *str16++,attr);
	}
}

UINT8
osdCharDraw(
	UINT8 x,
	UINT8 y,
	UINT16 idx,
	UINT8 attr
)
{
	UINT16 charW, charH;

	osdFontSizeGet(&charW, &charH);
	return osdCharDrawExt(x*charW, y*charH, idx,attr);
}

UINT8 osdMapTabSet(UINT8 idxBg, UINT8 idxOutLine, UINT8 idxBld0, UINT8 idxBld1)
{
	HAL_GprmOsdIdxSet(
		idxBg,
		idxOutLine,
		idxBld0,
		idxBld1);
	return 0;
}

void osdBldColorSet(UINT8 bldColor)
{
	idxBldColor = bldColor;
}

/**
 * @fn        UINT8 osdCharDrawExt(UINT8 posFmt, UINT16 x, UINT16 y, UINT16 idx)
 * @brief     osdCharDrawExt
 * @param     [in] x
 * @param     [in] y
 * @param     [in] idx
 * @retval    return = icon width
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-25
 * @todo      N/A
 * @bug       N/A
*/
UINT16
osdCharDrawExt(
	UINT16 x,
	UINT16 y,
	UINT16 idx,
	UINT8 bldOpt
)
{
#if 1 
	font_struct	font;
	UINT8* pbuf;
	UINT16 osdDispW, osdDispH;
	osdMem_t* pMem;
	UINT32 iconAddrWord;
    UINT32 dispAddrWord;
	UINT8 bTrsfm;
	UINT32 blindAddr;
	UINT32 blindBuffW=320;
	UINT32 blindBuffH=240;//32;

	osdDispBuffSizeGet(&osdDispW, &osdDispH);
	osdMemCbGet(&pMem);
	dispAddrWord = osdDispBuffAddrGet(1);
	blindAddr=pMem->datDraw.addr;

	if (idx < 0x80) {
		iconAddrWord = pMem->char8.addr;
	} else {
		iconAddrWord = pMem->char16.addr; 
		idx -= 0x80; }

	pbuf = (UINT8*) &font;
	
	HAL_DramRWBytes(iconAddrWord+(idx<<2),pbuf,8,1,DRAM_RWFormat_8);
	//HAL_DramDmaRead(&font, iconAddrWord+(idx<<2), 8, 1);

	font.font_h_pixel = pbuf[0];

	if (!font.font_h_pixel)
		return 0;

	if (font.font_w_word & 0x8000) {
		bTrsfm = 0; // 8bit
	} else {													 
		bTrsfm = 1;	// 2bit
	}
	font.font_w_word &= 0x7fff;
	font.font_offset &= 0x00ffffff;
	if((bldOpt) && (bTrsfm))
	{
		HAL_GprmOsdCpy(
			iconAddrWord+font.font_offset, font.font_w_word, font.font_h_pixel, 0, 0, 
			blindAddr, blindBuffW >> 1, blindBuffH, 0, 0,
			font.font_w_pixel, font.font_h_pixel,
			bTrsfm, 0, 0,
			1
		);	
		HAL_GprmOsdCpy(
			 blindAddr, blindBuffW >> 1, blindBuffH, 0, 0,
			dispAddrWord, osdDispW >> 1, osdDispH, x >> 1, y,
			font.font_w_pixel, font.font_h_pixel,
			0, 1, idxBldColor,
			1
		);	
	}
	else
	{ 
		HAL_GprmOsdCpy(
			iconAddrWord+font.font_offset, font.font_w_word, font.font_h_pixel, 0, 0, 
			dispAddrWord + (x >> 1), osdDispW >> 1, osdDispH, 0, y,
			font.font_w_pixel, font.font_h_pixel,
			bTrsfm, bldOpt, idxBldColor,
			1
		); 	
	}

	return (font.font_w_pixel<<1);
#endif

}

/**
 * @fn        UINT8 osdIconDrawExt(UINT8 posFmt, UINT16 x, UINT16 y, UINT16 idx)
 * @brief     osdIconDrawExt
 * @param     [in] x
 * @param     [in] y
 * @param     [in] idx
 * @retval    return = icon width
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-31
 * @todo      N/A
 * @bug       N/A
*/
UINT16
osdIconDrawExt(
	UINT16 x,
	UINT16 y,
	UINT16 idx,
	UINT8 bldOpt
)
{
	font_struct	font;
	UINT8* pbuf;
	UINT16 osdDispW, osdDispH;
	osdMem_t* pMem;
	UINT32 iconAddrWord;
    UINT32 dispAddrWord;
	UINT8 bTrsfm;
	UINT32 blindAddr;
	UINT32 blindBuffW=320;
	UINT32 blindBuffH=240;

	osdDispBuffSizeGet(&osdDispW, &osdDispH);
	osdMemCbGet(&pMem);	
	dispAddrWord = osdDispBuffAddrGet(1);
	blindAddr=pMem->datDraw.addr;
	iconAddrWord = pMem->icon.addr;

	pbuf = (UINT8*) &font;
	HAL_DramRWBytes(iconAddrWord+(idx<<2),pbuf,8,1,DRAM_RWFormat_8);
	//HAL_DramDmaRead(&font, iconAddrWord+(idx<<2), 8, 1);

	font.font_h_pixel = pbuf[0];
	if (!font.font_h_pixel)
		return 0; 

	if (font.font_w_word & 0x8000) {
		bTrsfm = 0; // 8bit
	} else {													 
		bTrsfm = 1;	// 2bit
	}
	font.font_w_word &= 0x7fff;
	font.font_offset &= 0x00ffffff;
	if((bldOpt) && (bTrsfm))
	{
		HAL_GprmOsdCpy(
			iconAddrWord+font.font_offset, font.font_w_word, font.font_h_pixel, 0, 0, 
			blindAddr, blindBuffW >> 1, blindBuffH, 0, 0,
			font.font_w_pixel, font.font_h_pixel,
			bTrsfm, 0, 0,
			1
		);	
		
		HAL_GprmOsdCpy(
			 blindAddr, blindBuffW >> 1, blindBuffH, 0, 0,
			dispAddrWord, osdDispW >> 1, osdDispH, x>>1, y,
			font.font_w_pixel, font.font_h_pixel,
			0, 1, idxBldColor,
			1
		);	
	}
	else
	{
		HAL_GprmOsdCpy(
			iconAddrWord+font.font_offset, font.font_w_word, font.font_h_pixel, 0, 0, 
			dispAddrWord + (x >> 1), osdDispW >> 1, osdDispH, 0, y,
			font.font_w_pixel, font.font_h_pixel,
			bTrsfm, bldOpt, idxBldColor,
			1
		);	
	}
	return (font.font_w_pixel<<1);
}
#if 1
UINT16
osdCharWidthGet(
	UINT16 idx
)
{
	font_struct	font;
	UINT8* pbuf;
	osdMem_t* pMem;
	UINT32 iconAddrWord;

	osdMemCbGet(&pMem);

	if (idx < 0x80) {
		iconAddrWord = pMem->char8.addr;
	} else {
		iconAddrWord = pMem->char16.addr; 
		idx -= 0x80; }

	pbuf = (UINT8*) &font;
	HAL_DramRWBytes(iconAddrWord+(idx<<2),pbuf,8,1,DRAM_RWFormat_8);
	//HAL_DramDmaRead(&font, iconAddrWord+(idx<<2), 8, 1);

	return (font.font_w_pixel<<1);
}

/**
 * @fn        UINT8 osdIconDrawExt(UINT8 posFmt, UINT16 x, UINT16 y, UINT16 idx)
 * @brief     osdIconDrawExt
 * @param     [in] x
 * @param     [in] y
 * @param     [in] idx
 * @retval    return = icon width
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-31
 * @todo      N/A
 * @bug       N/A
*/
UINT16
osdIconWidthGet(
	UINT16 idx
)
{
	font_struct	font;
	UINT8* pbuf;
	osdMem_t* pMem;
	UINT32 iconAddrWord;

	osdMemCbGet(&pMem);	
	iconAddrWord = pMem->icon.addr;

	pbuf = (UINT8*) &font;
	HAL_DramRWBytes(iconAddrWord+(idx<<2),pbuf,8,1,DRAM_RWFormat_8);
	//HAL_DramDmaRead(&font, iconAddrWord+(idx<<2), 8, 1);

	return (font.font_w_pixel<<1);
}

UINT16
osdIconHeightGet(
	UINT16 idx
)
{
	osdMem_t* pMem;
	UINT32 iconAddrWord;
	UINT8 valLow,valHig;
	
	osdMemCbGet(&pMem);	
	iconAddrWord = pMem->icon.addr;

	HAL_DramStartAddrSet(iconAddrWord+(idx<<2),1);
	HAL_DramWordDataRead(&valLow,&valHig);

	return (UINT16)valLow;
}

UINT16
osdStrWidthGet(
	UINT8 *str
)
{
	UINT16 w = 0;
	while ( *str ) {
		if (*str >= OSD_CHAR_BYTE_IDX_GRP)
			w += osdCharWidthGet(*str++);
		else
			w += osdCharWidthGet(*((UINT16*)str)++);
	}
	return w;
}
#endif
static UINT32 iconTabAddr;
void osdIconPsTabAddrSet(UINT32 addr)
{
	iconTabAddr = addr;
}
UINT32 osdIconPsTabAddrGet(void)
{
	return iconTabAddr;
}
/**************************************************************************
 *                                                                        *
 *  Function Name: sp1kOsdLoad                                            *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void osdIconPsGet(UINT16 idx, UINT16 *x,UINT16 *y)
{
	UINT32 addr=osdIconPsTabAddrGet();
	UINT8 valLow,valHig;
	
	HAL_DramStartAddrSet(addr+idx,1);
	HAL_DramWordDataRead(&valLow,&valHig);
	if(valLow==0xff || valHig == 0xff){
		*x = 0xA5;//0xffff;
		*y = 0xA5;//0xffff;
		//return;
	}else {
		*x = ((UINT16)valLow)<<1;
		*y = ((UINT16)valHig)<<1;
	}
}
/**************************************************************************
 *                                                                        *
 *  Function Name: sp1kOsdLoad                                            *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void osdIconShow(UINT16 iconId,UINT8 show)
{
//	UINT16 charW,charH;
	UINT16 x,y;
	
	osdIconPsGet(iconId,&x,&y);
	if(x==0xA5 || y == 0xA5)return;
	
	if(show){
		osdIconDrawExt(x, y, iconId,0/*attr*/);
	}else {
		//osdFontSizeGet(&charW, &charH);
		osdIndexSave();	
		osdPenCfg(1,1);
		osdRectDraw(x, y, osdIconWidthGet(iconId), osdIconHeightGet(iconId));
		osdIndexRestore();
	}
}
#endif /* MODULE_OSD_OPT */
