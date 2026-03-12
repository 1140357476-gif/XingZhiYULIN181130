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
 * @file      osd_line.c
 * @brief     draw line
 * @author    Phil Lin
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#define __FILE __FILE_ID_OSD_LINE__ 
#include "general.h"
#include "dbg_def.h"

#include "hal_gprm.h" 

#include "gprm.h" 
#include "osd_buff.h" 
#include "osd_mem.h" 
#include "osd_api.h" 
#include "reg_def.h"

#if MODULE_OSD_OPT
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

#define ABS(x,y)  ((x)>(y)) ? ((x)-(y)) : ((y)-(x))
#define MIN(x,y)  ((x)<(y)) ? (x) : (y)
#define MAX(x,y)  ((x)>(y)) ? (x) : (y)

#define OSD_PEN_PATT_BUFF_W_SET(addr)\
{\
	XBYTE[REG_Dram_IndirectAddr_B0] = READ8(&addr, 3);\
	XBYTE[REG_Dram_IndirectAddr_B1] = READ8(&addr, 2);\
	XBYTE[REG_Dram_IndirectAddr_B2] = READ8(&addr, 1);\
	XBYTE[REG_Dram_IndirectAddr_B3] = READ8(&addr, 0);\
	XBYTE[REG_Dram_IndirectPrefetch] = 0x00;\
}

#define OSD_PEN_PATT_W(LSB, MSB)\
{\
	while (XBYTE[REG_Dram_IndirectPrefetch]);\
	XBYTE[REG_Dram_IndirectRData_Low] = LSB;\
	XBYTE[REG_Dram_IndirectRData_High] = MSB;\
}


#define OSD_PEN_PATT_BUFF_R_SET(addr)\
{\
	XBYTE[REG_Dram_IndirectAddr_B0] = READ8(&addr, 3);\
	XBYTE[REG_Dram_IndirectAddr_B1] = READ8(&addr, 2);\
	XBYTE[REG_Dram_IndirectAddr_B2] = READ8(&addr, 1);\
	XBYTE[REG_Dram_IndirectAddr_B3] = READ8(&addr, 0);\
	XBYTE[REG_Dram_IndirectPrefetch] = 0x01;\
}

#define OSD_PEN_PATT_R(LSB, MSB)\
{\
	while (XBYTE[REG_Dram_IndirectPrefetch]);\
	LSB = XBYTE[REG_Dram_IndirectRData_Low];\
	MSB = XBYTE[REG_Dram_IndirectRData_High];\
}
 
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

/**
 * @fn        void osdDrawLineV(UINT16 x, UINT16 y, UINT16 w, UINT16 h, UINT8 dat)
 * @brief     osdDrawLineV
 * @param     [in] x
 * @param     [in] y
 * @param     [in] w
 * @param     [in] h
 * @param     [in] dat
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-24
 * @todo      N/A
 * @bug       N/A
*/
static void
osdDrawLineV(
	UINT16 x,
	UINT16 y,
	UINT16 w,
	UINT16 h,
	UINT8 dat
)
{
	UINT32 penPattAddrWord;
	UINT32 osdDispBuff = osdDispBuffAddrGet(1);
	UINT16 i;
	UINT8 msk = 0xff;
	UINT8 dat0, dat1;
	osdMem_t* pMem;
	UINT16 osdBuffW, osdBuffH;

	if (!w || !h) return;

	osdMemCbGet(&pMem);
	penPattAddrWord = pMem->datDraw.addr;
	
	OSD_PEN_PATT_BUFF_W_SET(penPattAddrWord);
	
	dat0 = dat1 = dat;
	if (x & 0x01) {
		dat0 = msk;
	} else {
		dat1 = msk;
	}
	for(i = 0; i < h; i++) {
		OSD_PEN_PATT_W(dat0, dat1);
	}

	osdDispBuffSizeGet(&osdBuffW, &osdBuffH);

#if 1 /* speed up */
	HAL_GprmOsdCpy(
		penPattAddrWord, 1, osdBuffH, 0, 0,
		osdDispBuff, osdBuffW >> 1, osdBuffH,
		x >> 1, y, w, h,
		0, 1, msk,
		1
	);
#else /* standard code */
	HAL_GprmOsdCpy(
		penPattAddrWord, 2 * 8 / 16, osdBuffH, 0, 0,
		osdDispBuff, osdBuffW * 8 / 16, osdBuffH,
		x * 8 / 16, y, w * 2 * 8 / 16, h,
		0, 1, msk,
		1
	);
#endif
}

/**
 * @fn        void osdDrawLineH(UINT16 x, UINT16 y, UINT16 w, UINT16 h, UINT8 dat)
 * @brief     osdDrawLineH
 * @param     [in] x
 * @param     [in] y
 * @param     [in] w
 * @param     [in] h
 * @param     [in] dat
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-24
 * @todo      N/A
 * @bug       N/A
*/
static void
osdDrawLineH(
	UINT16 x,
	UINT16 y,
	UINT16 w,
	UINT16 h,
	UINT8 dat
)
{
	UINT32 penPattAddrWord;
	UINT32 osdDispBuff = osdDispBuffAddrGet(1);
	UINT16 i;
	UINT8 msk = 0xff;
	UINT16 x0, x1;
	osdMem_t* pMem;
	UINT16 osdBuffW, osdBuffH;

	if (!w || !h) return;

	osdMemCbGet(&pMem);
	penPattAddrWord = pMem->datDraw.addr;

	OSD_PEN_PATT_BUFF_W_SET(penPattAddrWord);

	x0 = x;
	x1 = x + w;
	
	if (x0 & 0x01) {
		x0--;
		OSD_PEN_PATT_W(msk, dat);
		w--; //already write 1 patt
	}

	if (w < 1) {
		return; //the input value:w = 1
	} else {
		w = ((w - 1) & ~0x01);//w = 0, 2, 4, 6...		
	}
	for(i = 0; i < w; i+=2) {
		OSD_PEN_PATT_W(dat, dat);
	}
	
	if (x1 & 0x01) {
		x1++;
		OSD_PEN_PATT_W(dat, msk);
	} else {
		OSD_PEN_PATT_W(dat, dat);
	}
	
	x = x0;
	w = x1 - x0;
	osdDispBuffSizeGet(&osdBuffW, &osdBuffH);

#if 1 /* speed up */
	HAL_GprmOsdCpy(
		penPattAddrWord, osdBuffW >> 1, 1, 0, 0,
		osdDispBuff, osdBuffW >> 1, osdBuffH,
		x >> 1, y, w >> 1, h,
		0, 1, msk,
		1
	);
#else /* standard code */
	HAL_GprmOsdCpy(
		penPattAddrWord, osdBuffW * 8 / 16, 1, 0, 0,
		osdDispBuff, osdBuffW * 8 / 16, osdBuffH,
		x * 8 / 16, y, w * 8 / 16, h,
		0, 1, msk,
		1
	);
#endif
}


/**
 * @fn        UINT8 osdLineDraw(SINT16 x0, SINT16 y0, SINT16 x1, SINT16 y1)
 * @brief     osdLineDraw
 * @param     [in] x0
 * @param     [in] y0
 * @param     [in] x1
 * @param     [in] y1
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-24
 * @todo      N/A
 * @bug       N/A
*/
UINT8
osdLineDraw(
	SINT16 x0,
	SINT16 y0,
	SINT16 x1,
	SINT16 y1
)
{
	UINT16 i;
	UINT16 x, y, w, h;
	osdPen_t* pen;
	
	if (x0 != x1 && y0 != y1) {
		//Can't draw bial line
		ASSERT(0, 1);
		return FAIL;
	}

	osdPenGet(&pen);
	
	if (x0 == x1) {
		x = x0;
		y = MIN(y0, y1);
		w = pen->width;
		h = ABS(y0, y1);

		for (i = 0; i < w; i++) {
			osdDrawLineV(x++, y, 1, h, pen->color);
		}
	} else { //(y0 == y1)
		x = MIN(x0, x1);
		y = y0;
		w = ABS(x0, x1);
		h = pen->width;
		
		for (i = 0; i < h; i++) {
			osdDrawLineH(x, y++, w, 1, pen->color);
		}		
	}
	
	return SUCCESS;
}

/**
 * @fn        UINT8 osdFrameDraw(SINT16 x, SINT16 y, UINT16 w, UINT16 h)
 * @brief     osdFrameDraw
 * @param     [in] x
 * @param     [in] y
 * @param     [in] w
 * @param     [in] h
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-26
 * @todo      N/A
 * @bug       N/A
*/
UINT8
osdFrameDraw(
	SINT16 x,
	SINT16 y,
	UINT16 w,
	UINT16 h
)
{
	UINT16 i;
	osdPen_t* pen;
	
	osdPenGet(&pen);

	/* check frame size whether too small */
	if (w < pen->width || h < pen->width) {
		ASSERT(0, 1);
		return FAIL;
	}

	for (i = 0; i < pen->width; i++) {
		osdDrawLineH(x, y + i, w, 1, pen->color);
		osdDrawLineV(x + i, y, 1, h, pen->color);
		osdDrawLineH(x, y + h - pen->width + i, w, 1, pen->color);
		osdDrawLineV(x + w - pen->width + i, y, 1, h, pen->color);
	}
	
	return SUCCESS;
}
#endif /* MODULE_OSD_OPT */
