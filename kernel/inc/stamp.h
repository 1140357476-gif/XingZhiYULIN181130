#ifndef _STAMP_H__
#define _STAMP_H__
#include "general.h"
#include "hal_global.h"
#define STAMP_EN 1


#define STAMP_FONT_RES_NUM 	7
typedef struct
{
	UINT32 addr;
	UINT16 resId;
	UINT8 width,height;
	UINT8 count;
}stamp_font_t;

#define STAMP_TEXT_MAX_LEN	20
typedef struct
{
	UINT16 xOff,yOff;
	UINT8 text[STAMP_TEXT_MAX_LEN];//
	UINT32 imgAddrW,imgSize;//image addr painted string
	UINT16 imgWidth,imgHeight;
	UINT8 enable,pasteThd;
}stamp_text_t;

typedef enum
{
	STAMP_DATE=0,
	STAMP_TIME,
	STAMP_TEXT,
	
	STAMP_TYPE_MAX,
}stamp_text_type;


///////////Font
UINT8 stampFontResLoad(UINT8 id,UINT8 resId,UINT8 fontW,UINT8 fontH,UINT8 fontNum);

UINT8 stampFontInit();

stamp_font_t * stampFontSet(UINT32 fontAddr,UINT16 fontWidth,UINT16 fontHeight,UINT32 tmpAddr);

//////////Text
void stampTextReset();
/*
 * set date & time view format
 * @param   mode  : dateFmt: 0: yyyy/mm/dd
 *\n 						 1: dd/mm/yyyy
 *\n 						 2: mm/dd/yyyy
 *\n 						 3: yy/mm/dd
 *\n 			    timeFmt:   
 *\n 						0: hh:mm
*\n 						1: hh:mm AM/PM
*\n 						2: hh:mm:ss
*\n 						3: hh:mm:ss AM/PM
*\n 						4: hh:mm:ss A/P
*\n 						5: hh:mm A/P
*/
void stampDateTimeFmtSet(UINT8 dateFmt,UINT8 timeFmt);
/*
* create time string
* @param   mode  : fmt
*\n 						0: hh:mm
*\n 						1: hh:mm AM/PM
*\n 						2: hh:mm:ss
*\n 						3: hh:mm:ss AM/PM
*\n 						4: hh:mm:ss A/P
*\n 						5: hh:mm A/P
*/
void stampTimeFormat(UINT8* text,UINT8 hour,UINT8 minute,UINT8 second,UINT8 fmt);

/*
* create Date string 
* @param   mode  : fmt
*\n 0: yyyy/mm/dd
*\n 1: dd/mm/yyyy
*\n 2: mm/dd/yyyy
*\n 3: yy/mm/dd
*/
void stampDateFormat(UINT8* text,UINT8 year,UINT8 mon,UINT8 day,UINT8 fmt);

// Mode control: 0=photo(dot matrix), 1=video(original font)
void stampVideoModeSet(UINT8 video);

// Get current display resolution and auto-scale dot size
void stampResolutionGet(UINT16 *width, UINT16 *height);

//paint text to image at(xoff,yoff)
void stampPaintText(UINT32 addrW,UINT16 w,UINT16 h,UINT16 xOff,UINT16 yOff,UINT8 pasteThd,UINT8 *text);

//paint text to background image at(xoff,yof)
void stampPaintTextToBackground(UINT8* text,UINT32 xOff,UINT32 yOff,UINT8 pasteThd);

// Legacy dot matrix functions
void stampPaintDotMatrixDigit(UINT32 addrW, UINT16 w, UINT16 h, UINT16 xOff, UINT16 yOff, UINT8 pasteThd, UINT8 digit);
void stampPaintDotMatrixSymbol(UINT32 addrW, UINT16 w, UINT16 h, UINT16 xOff, UINT16 yOff, UINT8 pasteThd, UINT8 symbol);

//return stamp background image addr/width/height
void stampBackgroundGet(UINT32* addrW,UINT32* xSize,UINT32* ySize);

//set stamp background image addr/width/height
void stampBackgroundSet(UINT32 addrW,UINT16 xSize,UINT16 ySize);

/*
void stampImageSet(UINT32 addrW,UINT16 xSize,UINT16 ySize,UINT16 xOff,UINT16 yOff);
*/

//set switch, that copy background image to stamp or not,when combine
UINT8 stampBackgroundEnable(UINT8 enable);

//set switch, that paint text selected to stamp or not,when combine
UINT8 stampTextEnable(stamp_text_type type,UINT8 enable);

/*set text property
* @param  : 
* xoff,yoff: text painting postion
* pasteThd: 
*/
void stampTextSet(stamp_text_type type,UINT32 xOff,UINT32 yOff,UINT8 pasteThd);

stamp_text_t* stampTextGet(stamp_text_type type);

/*set image of text property
* @param  : 
* imgAddr:	 image of text 
*  width, height: 
*/
void stampTextImageSet(stamp_text_type type,UINT32 imgAddr,UINT32 size);//UINT16 width,UINT16 height);


/*modify text using string, update differnce only
*/
void stampTextModifyByString(stamp_text_type type,UINT8* str);

/*modify image of text
*/
void stampTextModifyByImage(stamp_text_type type,UINT32 imgAddr,UINT16 imgw,UINT16 imgh);


/*modify date & time by gtimer
* param: gtimer==0, use current gtimer
*/
void stampTextModifyByGTimer(UINT32 gTimer);


/*modify date & time by RTC
* param: rtc==0, use current RTC
*/
void stampTextModifyByRTC(dateStc_t* rtc);


/*combine background and image of text to dstImage
* param: dstAddrW
*/
void stampCombine(UINT32 dstAddrW);


/*combine background and image of text to dstImage at (xoff,yoff)
* param: dstAddrW,w,h,xoff,yoff
*/
void stampCombineToImage(UINT32 dstAddrW,UINT16 w,UINT16 h,UINT16 xoff,UINT16 yoff,UINT8 pasteThr);


/*update date& time by RTC,combine background and image of text to dstImage
* param: dstAddrW
*/
void stampCombineNext(UINT32 dstAddrW);


/*update date& time by skipSencond
* param: 
*/
void stampDateTimeUpdate(dateStc_t* dateInfNext,UINT32 skipSecond);


/*update date& time by 1 skipSencond
* param: 
*/
void stampDateTimeNext(dateStc_t* dateInfNext);

/*return date time info*/
dateStc_t* stampRTCInfoGet();

/*return gtimer*/
UINT32 stampGTimerGet();

// Video mode date stamp initialization (based on 1.C implementation)
void stampVideoModeInit(UINT32 fontAddr, UINT16 fontWidth, UINT16 fontHeight, UINT32 stampAddr, 
					 UINT32 imgAddr, UINT16 stampW, UINT16 stampH, UINT32 tmpAddr);

#endif
