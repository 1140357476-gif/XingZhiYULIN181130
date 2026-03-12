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
#ifndef __SP1K_STAMP_API_H__
#define __SP1K_STAMP_API_H__

/** \file sp1k_stamp_api.h
 * @brief		Spca1628 stamp related interface
 */
#include "stamp.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 #ifdef SP1K_DOXY_GEN
 ////////////////////////////
 //Font
 ////////////////////////////
 /**
 * @fn        UINT8 sp1kStampFontInit(void)
 * @brief     initialize stamp font using default param
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Wangjg
 * @since     2011-8-11
 * @todo      N/A
 * @bug       N/A
*/
UINT8 sp1kStampFontInit();
 
/**
 * @fn        UINT8 sp1kStampFontResLoad(UINT8 id, id,UINT8 resId, resId,UINT8 fontW, tW,UINT8 fontH, tH,UINT8 fontNum)
 * @brief     load res to stamp font list
 * @param     [in] id		:font list index,max=STAMP_FONT_RES_NUM
 * @param     [in] resId
 * @param     [in] fontW
 * @param     [in] fontH
 * @param     [in] fontNum	:number of font in res
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Wangjg
 * @since     2011-8-11
 * @todo      N/A
 * @bug       N/A
*/
UINT8 sp1kStampFontResLoad(UINT8 id,UINT8 resId,UINT8 fontW,UINT8 fontH,UINT8 fontNum);

/**
 * @fn        void sp1kStampFontSet(UINT32 fontAddr, UINT16 fontWidth, UINT16 fontHeight, UINT32 tmpAddr)
 * @brief     set font infomation
 * @param     [in] fontAddr
 * @param     [in] fontWidth
 * @param     [in] fontHeight
 * @param     [in] tmpAddr
 * @retval    stamp_font_t struct selected
 * @see       NULL
 * @author    Wangjg
 * @since     2011-8-10
 * @todo      N/A
 * @bug       N/A
*/
stamp_font_t* sp1kStampFontSet(UINT32 fontAddr,UINT16 fontWidth,UINT16 fontHeight,UINT32 tmpAddr);

////////////////////////////////////
//Text
/////////////////////////////////////
/**
 * @fn        void sp1kStampReset(void)
 * @brief     Reset stamp all global param to 0
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2011-8-10
 * @todo      N/A
 * @bug       N/A
*/
void sp1kStampTextReset();

/**
 * @fn        UINT8 sp1kStampDateTimeFmtSet(UINT8 dateFmt, dateFmt,UINT8 timeFmt)
 * @brief      * set date & time view format
 * @param     [in] dateFmt	0: yyyy/mm/dd
 *\n 						 1: dd/mm/yyyy
 *\n 						 2: mm/dd/yyyy
 *\n 						 3: yy/mm/dd
 * @param     [in] timeFmt	
*\n 						0: hh:mm
*\n 						1: hh:mm AM/PM
*\n 						2: hh:mm:ss
*\n 						3: hh:mm:ss AM/PM
*\n 						4: hh:mm:ss A/P
*\n 						5: hh:mm A/P
 * @retval    return null
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-8-10
 * @todo      N/A
 * @bug       N/A
*/
void sp1kStampDateTimeFmtSet(UINT8 dateFmt,UINT8 timeFmt);


/**
 * @fn        void sp1kStampTimeFormat(UINT8* text, text,UINT8 hour, hour,UINT8 minute, ute,UINT8 second, d,UINT8 fmt)
 * @brief     create time string
 * @param     [out] text
 * @param     [in] hour
 * @param     [in] minute
 * @param     [in] second
 * @param     [in] fmt		
*\n 						0: hh:mm
*\n 						1: hh:mm AM/PM
*\n 						2: hh:mm:ss
*\n 						3: hh:mm:ss AM/PM
*\n 						4: hh:mm:ss A/P
*\n 						5: hh:mm A/P
 * @retval    NONE
 * @see       NULL
 * @author    Wangjg
 * @since     2011-8-10
 * @todo      N/A
 * @bug       N/A
*/
void sp1kStampTimeFormat(UINT8* text,UINT8 hour,UINT8 minute,UINT8 second,UINT8 fmt);


/**
 * @fn        void sp1kStampDateFormat(UINT8* text, text,UINT16 year, year,UINT8 mon, ,UINT8 day, day,UINT8 fmt)
 * @brief     create Date string 
 * @param     [out] text
 * @param     [in] year
 * @param     [in] mon
 * @param     [in] day
 * @param     [in] fmt 	0: yyyy/mm/dd
 *\n 					1: dd/mm/yyyy
 *\n					2: mm/dd/yyyy
 *\n 					3: yy/mm/dd
 * @retval    NONE
 * @see       NULL
 * @author    Wangjg
 * @since     2011-8-10
 * @todo      N/A
 * @bug       N/A
*/
void sp1kStampDateFormat(UINT8* text,UINT16 year,UINT8 mon,UINT8 day,UINT8 fmt);


/**
 * @fn        void sp1kStampPaintText(UINT32 addrW, addrW,UINT16 w, w,UINT16 h, h,UINT16 xOff, xOff,UINT16 yOff, yOff,UINT8 pasteThd, pasteThd,UINT8 *text)
 * @brief     paint text to image at(xoff,yoff)
 * @param     [in] addrW
 * @param     [in] w
 * @param     [in] h
 * @param     [in] xOff
 * @param     [in] yOff
 * @param     [in] pasteThd
 * @param     [in] text
 * @retval    NONE
 * @see       NULL
 * @author    Wangjg
 * @since     2011-8-10
 * @todo      N/A
 * @bug       N/A
*/
void sp1kStampPaintText(UINT32 addrW,UINT16 w,UINT16 h,UINT16 xOff,UINT16 yOff,UINT8 pasteThd,UINT8 *text);


/**
 * @fn        void sp1kStampPaintTextToBackground(UINT8* text, text,UINT32 xOff, xOff,UINT32 yOff, yOff,UINT8 pasteThd)
 * @brief     paint text to background image at(xoff,yof)
 * @param     [in] text
 * @param     [in] xOff
 * @param     [in] yOff
 * @param     [in] pasteThd
 * @retval    NONE
 * @see       NULL
 * @author    Wangjg
 * @since     2011-8-10
 * @todo      N/A
 * @bug       N/A
*/
void sp1kStampPaintTextToBackground(UINT8* text,UINT32 xOff,UINT32 yOff,UINT8 pasteThd);


/**
 * @fn        void sp1kStampBackgroundGet(UINT32* addrW, addrW,UINT32* xSize, xSize,UINT32* ySize)
 * @brief     get stamp background image addr/width/height
 * @param     [in] addrW
 * @param     [in] xSize
 * @param     [in] ySize
 * @retval    NONE
 * @see       NULL
 * @author    Wangjg
 * @since     2011-8-10
 * @todo      N/A
 * @bug       N/A
*/
void sp1kStampBackgroundGet(UINT32* addrW,UINT32* xSize,UINT32* ySize);


/**
 * @fn        void sp1kStampBackgroundSet(UINT32 addrW, addrW,UINT16 xSize, xSize,UINT16 ySize)
 * @brief     set stamp background image addr/width/height
 * @param     [in] addrW
 * @param     [in] xSize
 * @param     [in] ySize
 * @retval    NONE
 * @see       NULL
 * @author    Wangjg
 * @since     2011-8-10
 * @todo      N/A
 * @bug       N/A
*/
void sp1kStampBackgroundSet(UINT32 addrW,UINT16 xSize,UINT16 ySize);

/*
void sp1kStampImageSet(UINT32 addrW,UINT16 xSize,UINT16 ySize,UINT16 xOff,UINT16 yOff);
*/

/**
 * @fn        void sp1kStampBackgroundEnable(UINT8 enable)
 * @brief    do copy background image to sp1kStamp or not,when combine
 * @param     [in] enable
 * @retval    NONE
 * @see       NULL
 * @author    Wangjg
 * @since     2011-8-10
 * @todo      N/A
 * @bug       N/A
*/
void sp1kStampBackgroundEnable(UINT8 enable);


/**
 * @fn        void sp1kStampTextEnable(stamp_text_type type, type,UINT8 enable)
 * @brief    do paint text selected to stamp or not,when combine
 * @param     [in] type
 * @param     [in] enable
 * @retval    NONE
 * @see       NULL
 * @author    Wangjg
 * @since     2011-8-10
 * @todo      N/A
 * @bug       N/A
*/
void sp1kStampTextEnable(stamp_text_type type,UINT8 enable);

/**
 * @fn        void sp1kStampTextSet(stamp_text_type type, type,UINT32 xOff, xOff,UINT32 yOff, yOff,UINT8 pasteThd)
 * @brief     set text property
 * @param     [in] type
 * @param     [in] xOff
 * @param     [in] yOff
 * @param     [in] pasteThd
 * @retval    NONE
 * @see       NULL
 * @author    Wangjg
 * @since     2011-8-10
 * @todo      N/A
 * @bug       N/A
*/
void sp1kStampTextSet(stamp_text_type type,UINT32 xOff,UINT32 yOff,UINT8 pasteThd);

/**
 * @fn        void sp1kStampTextImageSet(stamp_text_type type, type,UINT32 imgAddr, imgAddr,UINT32 size)
 * @brief     set image of text property
 * @param     [in] type
 * @param     [in] imgAddr:	image of text 
 * @param     [in] size
 * @retval    NONE
 * @see       NULL
 * @author    Wangjg
 * @since     2011-8-10
 * @todo      N/A
 * @bug       N/A
*/
void sp1kStampTextImageSet(stamp_text_type type,UINT32 bufAddr,UINT32 size);//UINT16 width,UINT16 height);

/**
 * @fn        void sp1kStampTextModifyByString(stamp_text_type type, type,UINT8* str)
 * @brief    modify text using string, update differnce only
 * @param     [in] type
 * @param     [in] str
 * @retval    NONE
 * @see       NULL
 * @author    Wangjg
 * @since     2011-8-10
 * @todo      N/A
 * @bug       N/A
*/
void sp1kStampTextModifyByString(stamp_text_type type,UINT8* str);

/**
 * @fn        void sp1kStampTextModifyByImage(stamp_text_type type, type,UINT32 imgAddr)
 * @brief    modify image of text
 * @param     [in] type
 * @param     [in] imgAddr
 * @param     [in] imgw	:if equal 0,used preset value
 * @param     [in] imgh	:if equal 0,used preset vlaue
 * @retval    NONE
 * @see       NULL
 * @author    Wangjg
 * @since     2011-8-10
 * @todo      N/A
 * @bug       N/A
*/
void sp1kStampTextModifyByImage(stamp_text_type type,UINT32 imgAddr,UINT16 imgw,UINT16 imgh);


/**
 * @fn        void sp1kStampTextModifyByGTimer(UINT32 gTimer)
 * @brief     modify date & time by gtimer
 * @param     [in] gTimer:	gtimer==0, use current gtimer
 * @retval    NONE
 * @see       NULL
 * @author    Wangjg
 * @since     2011-8-10
 * @todo      N/A
 * @bug       N/A
*/
void sp1kStampTextModifyByGTimer(UINT32 gTimer);


/**
 * @fn        void stampTextModifyByRTC(dateStc_t* rtc)
 * @brief     modify date & time by RTC
 * @param     [in] dateStc_t* rtc:	rtc==0, use current RTC
 * @retval    NONE
 * @see       NULL
 * @author    Wangjg
 * @since     2011-8-10
 * @todo      N/A
 * @bug       N/A
*/
void stampTextModifyByRTC(dateStc_t* rtc);


/**
 * @fn        void sp1kStampCombine(UINT32 dstAddrW)
 * @brief     combine background and image of text to dstImage
 * @param     [in] dstAddrW
 * @retval    NONE
 * @see       NULL
 * @author    Wangjg
 * @since     2011-8-10
 * @todo      N/A
 * @bug       N/A
*/
void sp1kStampCombine(UINT32 dstAddrW);



/**
 * @fn        void sp1kStampCombineToImage(UINT32 dstAddrW, dstAddrW,UINT16 w, w,UINT16 h, h,UINT16 xoff, xoff,UINT16 yoff)
 * @brief     combine background and image of text to dstImage at (xoff,yoff)
 * @param     [in] dstAddrW
 * @param     [in] w
 * @param     [in] h
 * @param     [in] xoff
 * @param     [in] yoff
 * @retval    NONE
 * @see       NULL
 * @author    Wangjg
 * @since     2011-8-10
 * @todo      N/A
 * @bug       N/A
*/
void sp1kStampCombineToImage(UINT32 dstAddrW,UINT16 w,UINT16 h,UINT16 xoff,UINT16 yoff);


/**
 * @fn        void sp1kStampCombineNext(UINT32 dstAddrW)
 * @brief     update date& time by RTC,combine background and image of text to dstImage
 * @param     [in] dstAddrW
 * @retval    NONE
 * @see       NULL
 * @author    Wangjg
 * @since     2011-8-10
 * @todo      N/A
 * @bug       N/A
*/
void sp1kStampCombineNext(UINT32 dstAddrW);


/**
 * @fn        void sp1kStampDateTimeUpdate(dateStc_t* dateInfNext, fNext,UINT32 skipSecond)
 * @brief     update date& time by skipSencond
 * @param     [in] dateInfNext
 * @param     [in] skipSecond
 * @retval    NONE
 * @see       NULL
 * @author    Wangjg
 * @since     2011-8-10
 * @todo      N/A
 * @bug       N/A
*/
void sp1kStampDateTimeUpdate(dateStc_t* dateInfNext,UINT32 skipSecond);


/**
 * @fn        void sp1kStampDateTimeNext(dateStc_t* dateInfNext)
 * @brief     update date& time by 1 skipSencond
 * @param     [in] dateInfNext
 * @retval    NONE
 * @see       NULL
 * @author    Wangjg
 * @since     2011-8-10
 * @todo      N/A
 * @bug       N/A
*/
void sp1kStampDateTimeNext(dateStc_t* dateInfNext);


#else
#define sp1kStampFontInit stampFontInit
#define sp1kStampFontResLoad stampFontResLoad 
#define sp1kStampFontSet stampFontSet

#define sp1kStampTextReset stampTextReset
#define sp1kStampDateTimeFmtSet stampDateTimeFmtSet
#define sp1kStampTimeFormat stampTimeFormat
#define sp1kStampDateFormat stampDateFormat
#define sp1kStampPaintText stampPaintText
#define sp1kStampPaintTextToBackground stampPaintTextToBackground
#define sp1kStampBackgroundGet stampBackgroundGet
#define sp1kStampBackgroundSet stampBackgroundSet
//#define sp1kStampImageSet stampImageSet
#define sp1kStampBackgroundEnable stampBackgroundEnable
#define sp1kStampTextEnable stampTextEnable
#define sp1kStampTextSet stampTextSet
#define sp1kStampTextImageSet stampTextImageSet
#define sp1kStampTextModifyByString stampTextModifyByString
#define sp1kStampTextModifyByImage stampTextModifyByImage
#define sp1kStampTextModifyByGTimer stampTextModifyByGTimer
#define sp1kStampTextModifyByRTC  stampTextModifyByRTC
#define sp1kStampCombine stampCombine
#define sp1kStampCombineToImage stampCombineToImage
#define sp1kStampCombineNext stampCombineNext
#define sp1kStampDateTimeUpdate stampDateTimeUpdate
#define sp1kStampDateTimeNext stampDateTimeNext
#endif
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

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

#endif //__SP1K_STAMP_API_H__

