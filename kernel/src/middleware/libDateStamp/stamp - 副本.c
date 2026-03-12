/**************************************************************************
 * *
 * Copyright (c) 2007 by Sunplus mMedia Inc.                      *
 * *
 * This software is copyrighted by and is the property of Sunplus        *
 * mMedia Inc. All rights are reserved by Sunplus mMedia Inc.  This      *
 * software may only be used in accordance with the corresponding        *
 * license agreement. Any unauthorized use, duplication, distribution,   *
 * or disclosure of this software is expressly forbidden.                *
 * *
 **************************************************************************/
/**
 * @file		datestamp.c
 * @brief		Unified stamp API for RTC init (Photo: Dot Matrix, Video: Font)
 */
//=============================================================================
//Header file
//=============================================================================
#include "general.h"
#include "hal_gprm.h"
#include "dbg_def.h"
#include "stamp.h"
#include "initio.h"
#include "sp1k_rsvblk_api.h"
#include "utility.h"
#include "sp1k_snap_api.h"

#if STAMP_EN
#define __FILE	__FILE_ID_DATESTAMP__

// External function declaration
extern void dispPnlDimGet(UINT16 *dispH, UINT16 *dispW);

// Mode control: 0=photo(dot matrix) 1=video(original font)
static UINT8 gStampVideoMode = 0;

// Dot matrix config
#define DOT_ROWS 7
#define DOT_COLS 5

// 点阵颜色配置 - 修正 YUV 值 
#define DOT_COLOR_YELLOW    0 
#define DOT_COLOR_GREEN     1 
#define DOT_COLOR_WHITE     2 
#define DOT_COLOR_RED       3 

// 修正的 YUV 值 (YUV422 格式) 
static const UINT8 colorTable[4][3] = { 
    {0xD2, 0x10, 0x92},  // 黄色 (Y=210, U=16, V=146) 
    {0x96, 0x2E, 0x15},  // 绿色 (Y=150, U=46, V=21)  
    {0xFF, 0x80, 0x80},  // 白色 (Y=255, U=128, V=128) 
    {0x51, 0x5F, 0xF0},  // 红色 (Y=81, U=95, V=240) 
};

static UINT8 dotSize = 8;
static UINT8 dotSpace = 3;
static UINT8 lastDotSize = 0;  
static UINT8 dotInit = 0;  
static UINT8 lastDotColor = DOT_COLOR_YELLOW;  
static UINT8 dotColor = DOT_COLOR_YELLOW;  

// Function prototypes (static)
static void stampAutoScale(UINT16 w, UINT16 h);
static void stampDotInit(void);
static void stampDrawDot(UINT32 addrW, UINT16 w, UINT16 h, UINT16 x, UINT16 y, UINT8 pasteThd);
static void stampPaintPattern(UINT32 addrW, UINT16 w, UINT16 h, UINT16 xOff, UINT16 yOff, UINT8 pasteThd, const UINT8 *pattern);

stamp_font_t stampFonts[STAMP_FONT_RES_NUM];

UINT32 stampFontAddr;
UINT16 stampFontWidth,stampFontHeight;
UINT8 stampFontNum;
UINT32 stampBGAddr;
UINT16 stampBGWidth,stampBGHeight;
UINT32 stampImgAddr;
UINT8  stampBGEnable;
UINT8  stampDateFmt;
UINT8 stampTimeFmt;
stamp_text_t dtText[STAMP_TYPE_MAX];
dateStc_t stampDateInf;
UINT32 stampGTimer;

void stampTextReset()
{
	memset(dtText,0,sizeof(dtText));
}

UINT8 stampFontResLoad(UINT8 id,UINT8 resId,UINT8 fontW,UINT8 fontH,UINT8 fontNum)
{
	stamp_font_t *pFont = stampFonts+id-1;
	UINT32 addr = K_SDRAM_GrafFontBufAddr;
	if(id>STAMP_FONT_RES_NUM){return FALSE;}
	if(id>0){
		if(pFont->addr==0){return FALSE;}
		addr= pFont->addr+ (UINT32)pFont->width* pFont->height* pFont->count;
	}
	if(addr+(UINT32)fontW*fontH*fontNum - K_SDRAM_GrafFontBufAddr > K_SDRAM_GrafFontBufSize){return FALSE;}
	pFont++;
	pFont->addr = addr;
	pFont->resId = resId;
	pFont->width = fontW;
	pFont->height = fontH;	
	pFont->count = fontNum;
	sp1kNandRsvRead(resId, addr<<1);
	return TRUE;
}

UINT8 stampFontInit()
{	
	UINT8 fonts[]={
		 0x23,32,64,16
		,0x24,24,48,16
		,0x25,16,32,16
		,0x26,12,24,16
		,0x27,10,20,16
		,0x28,8,16,16
		,0x29,6,10,16
	};
	UINT8 *pfont = fonts;
	UINT8 i,ret;
	for(i=0;i<STAMP_FONT_RES_NUM;i++,pfont+=4)
	{		
		ret = stampFontResLoad(i,pfont[0],pfont[1],pfont[2],pfont[3]);
	}
	return ret;
}

stamp_font_t* stampFontSet(UINT32 fontAddr,UINT16 fontWidth,UINT16 fontHeight,UINT32 tmpAddr)
{
	UINT8 i;
	stamp_font_t *pFont = stampFonts;
	stampFontAddr = fontAddr;
	stampFontWidth = fontWidth;
	stampFontHeight = fontHeight;	
	
	for(i=0;i<7;i++,pFont++){
		if(fontWidth >= pFont->width && fontHeight >= pFont->height){
			break;
		}
	}

	if(i>=7){i=6;pFont--;}
	
	stampFontNum = pFont->count;
	if(fontAddr && tmpAddr){
		HAL_GprmScale(pFont->addr
			, (UINT16)pFont->width* (UINT16)stampFontNum, (UINT16)pFont->height,
			fontAddr
			, (UINT16)fontWidth * (UINT16)stampFontNum 
			, (UINT16)fontHeight, tmpAddr
			, 0);
	}
	return pFont;
}

void stampDateTimeFmtSet(UINT8 dateFmt,UINT8 timeFmt)
{
	stampDateFmt = dateFmt;
	stampTimeFmt = timeFmt;
}

void stampTimeFormat(UINT8* text,UINT8 hour,UINT8 minute,UINT8 second,UINT8 fmt)
{
	UINT8 ai = 0;
	UINT8 pattern[]= "%02bu:%02bu:%02bu AM";
	
	switch(fmt){
	case 0:
		pattern[11] = 0;
		break;
	case 1:
		ai = 12;
		pattern[11] = ' ';
		pattern[12] = 'A';
		pattern[13] = 'M';
		pattern[14] = 0;
		break;
	case 2:
		pattern[17] = 0;
		break;
	case 3:
		ai = 18;
		break;
	case 4:
		ai = 18;
		pattern[19] = 0;
		break;	
	case 5:
		ai = 12;
		pattern[11] = ' ';
		pattern[12] = 'A';
		pattern[13] = 0;
		break;
	default:
		pattern[17] = 0;
		break;
	}
	if(ai && hour>=12){	
		if(hour>12){
			hour = hour - 12;	
		}
		pattern[ai] = 'P';	
	}
	else if(ai && hour==0){
		hour=12;
		pattern[ai] = 'A';	
	}

	sprintf(text,pattern,hour,minute,second);	
}

void stampDateFormat(UINT8* text,UINT8 year,UINT8 mon,UINT8 day,UINT8 fmt)
{
	code UINT8 pattern0[]= "20%02bu/%02bu/%02bu";
	code UINT8 pattern1[]= "%02bu/%02bu/20%02bu";
	code UINT8 pattern2[]= "%02bu/%02bu/%02bu";
	switch( fmt ){
	case 0://year/mon/day
		sprintf(text,pattern0,year,mon,day);
		break;
	case 1://day/mon/year
		sprintf(text,pattern1,day,mon,year);
		break;
	case 2://mon/day/year
		sprintf(text,pattern1,mon,day,year);
		break;
	case 3://year/mon/day
		sprintf(text,pattern2,year,mon,day);
		break;
	default://year/mon/day
		sprintf(text,pattern0,year,mon,day);
		break;
	}
}

static UINT32 stampFontHOffsetGet(UINT8 val)
{
	UINT32 hoff;
	if(val >= '/' && val <= ':'){
		val = val - '/'; 
	}
	else {
		switch (val){                    
		case 'A': val = ':' - '/' + 1; break;
		case 'P': val = ':' - '/' + 2; break;
		case 'M': val = ':' - '/' + 3; break;                     
		default:  val = stampFontNum-1; break;
		}
	}
	hoff = (UINT16)val * stampFontWidth;
	return hoff;
}

// 7x5 digit patterns 
static const UINT8 digitPatterns[10][DOT_ROWS] = {
 {0x0E,0x11,0x13,0x15,0x19,0x11,0x0E},
 {0x04,0x0C,0x04,0x04,0x04,0x04,0x0E},
 {0x0E,0x11,0x01,0x02,0x04,0x08,0x1F},
 {0x1E,0x01,0x01,0x0E,0x01,0x01,0x1E},
 {0x02,0x06,0x0A,0x12,0x1F,0x02,0x02},
 {0x1F,0x10,0x1E,0x01,0x01,0x11,0x0E},
 {0x06,0x08,0x10,0x1E,0x11,0x11,0x0E},
 {0x1F,0x01,0x02,0x04,0x08,0x08,0x08},
 {0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E},
 {0x0E,0x11,0x11,0x0F,0x01,0x02,0x0C}
};

static const UINT8 slashPattern[DOT_ROWS] = {0x02,0x02,0x04,0x04,0x08,0x10,0x10};
static const UINT8 colonPattern[DOT_ROWS] = {0x00,0x04,0x04,0x00,0x04,0x04,0x00};
static const UINT8 dotPattern[DOT_ROWS]   = {0x00,0x00,0x00,0x00,0x00,0x04,0x00};

static UINT8 dotBuffer[4096]; 

static void stampAutoScale(UINT16 w, UINT16 h)
{
	UINT16 actualW, actualH;
	UINT32 pixels;
	// h参数未使用，直接忽略
	
	if (gStampVideoMode == 0) { 
		sp1kSnapSizeGet(&actualW, &actualH);
		pixels = (UINT32)actualW * (UINT32)actualH;
		
		if(pixels >= 8160UL*6120UL) { dotSize = 16; dotSpace = 4; }
		else if(pixels >= 7664UL*5748UL) { dotSize = 15; dotSpace = 4; }
		else if(pixels >= 7488UL*5616UL) { dotSize = 15; dotSpace = 4; }
		else if(pixels >= 6944UL*5208UL) { dotSize = 14; dotSpace = 4; }
		else if(pixels >= 5600UL*4200UL) { dotSize = 12; dotSpace = 3; }
		else if(pixels >= 5120UL*3840UL) { dotSize = 11; dotSpace = 3; }
		else if(pixels >= 4608UL*3456UL) { dotSize = 10; dotSpace = 3; }
		else if(pixels >= 3888UL*2916UL) { dotSize = 9; dotSpace = 2; }
		else if(pixels >= 3664UL*2748UL) { dotSize = 8; dotSpace = 2; }
		else if(pixels >= 3264UL*2448UL) { dotSize = 8; dotSpace = 2; }
		else if(pixels >= 2592UL*1944UL) { dotSize = 7; dotSpace = 2; }
		else if(pixels >= 2048UL*1536UL) { dotSize = 6; dotSpace = 2; }
		else if(pixels >= 1600UL*1200UL) { dotSize = 5; dotSpace = 2; }
		else if(pixels >= 1920UL*1080UL) { dotSize = 5; dotSpace = 2; }
		else if(pixels >= 1280UL*720UL) { dotSize = 4; dotSpace = 2; }
		else if(pixels >= 1024UL*768UL) { dotSize = 4; dotSpace = 1; }
		else if(pixels >= 640UL*480UL) { dotSize = 3; dotSpace = 1; }
		else { dotSize = 2; dotSpace = 1; }
		
		if (dotSize & 1) dotSize++;
		if (dotSpace & 1) dotSpace++;
	} else { 
		// 录像模式：根据 OSD 窗口宽度(w) 自动缩放点阵，防止超长越界
		if (w >= 1920) {
			dotSize = 8; dotSpace = 3;  // 1080P - 增大字体
		} else if (w >= 1280) {
			dotSize = 6; dotSpace = 2;  // 720P - 增大字体
		} else if (w >= 800) {
			dotSize = 4; dotSpace = 1;  // WVGA - 增大字体
		} else if (w >= 640) {
			dotSize = 3; dotSpace = 1;  // VGA - 增大字体
		} else {
			dotSize = 2; dotSpace = 0;  // 更小分辨率 - 增大字体
		}
		
		// 确保dotSize至少为1，避免绘制错误
		if (dotSize < 1) dotSize = 1;
	}
}

static void stampDotInit(void)
{
    UINT32 i;
    UINT32 totalPixels;
    UINT32 bufferSize;
    UINT8 y, u, v;
    UINT16 *pBuf16;  
    UINT8 *pUncached;

    if(dotInit && lastDotSize == dotSize && lastDotColor == dotColor) { return; }
    if (dotSize & 1) dotSize++;

    totalPixels = (UINT32)dotSize * dotSize;
    
    y = colorTable[dotColor][0];
    u = colorTable[dotColor][1];
    v = colorTable[dotColor][2];

    pUncached = dotBuffer;
    pBuf16 = (UINT16 *)pUncached;

    // 确保缓冲区足够大，避免溢出
    bufferSize = totalPixels * 2;
    if (bufferSize > sizeof(dotBuffer)) {
        bufferSize = sizeof(dotBuffer);
    }

    for(i = 0; i < bufferSize; i += 4) {
        dotBuffer[i]   = y;   
        dotBuffer[i+1] = u;   
        dotBuffer[i+2] = y;   
        dotBuffer[i+3] = v;   
    }

    lastDotSize = dotSize;
    lastDotColor = dotColor;
    dotInit = 1;
}

void stampVideoModeSet(UINT8 video)
{
	gStampVideoMode = video;
}

void stampDotColorSet(UINT8 color)
{
	if(color <= DOT_COLOR_RED) {
		if(dotColor != color) {
			dotColor = color;
			dotInit = 0; 
			lastDotColor = (color == DOT_COLOR_YELLOW) ? DOT_COLOR_GREEN : DOT_COLOR_YELLOW;
		}
	}
}

void stampResolutionGet(UINT16 *width, UINT16 *height)
{
	dispPnlDimGet(height, width);
	stampAutoScale(*width, *height);
}

void stampPaintDotMatrixDigit(UINT32 addrW, UINT16 w, UINT16 h, UINT16 xOff, UINT16 yOff, UINT8 pasteThd, UINT8 digit)
{
	if(digit > 9) return;
	stampAutoScale(w, h);
	stampDotInit();
	stampPaintPattern(addrW, w, h, xOff, yOff, pasteThd, digitPatterns[digit]);
}

void stampPaintDotMatrixSymbol(UINT32 addrW, UINT16 w, UINT16 h, UINT16 xOff, UINT16 yOff, UINT8 pasteThd, UINT8 symbol)
{
	const UINT8 *pattern;
	
	stampAutoScale(w, h);
	stampDotInit();
	
	if(symbol == 0) pattern = slashPattern;
	else if(symbol == 1) pattern = colonPattern;
	else if(symbol == 2) pattern = dotPattern;
	else return;
	
	stampPaintPattern(addrW, w, h, xOff, yOff, pasteThd, pattern);
}

// 绘画底层：做严格的分支处理，录像拷贝字库YUV，拍照绘制点阵
void stampPaintText(UINT32 addrW,UINT16 w,UINT16 h,UINT16 xOff,UINT16 yOff,UINT8 pasteThd,UINT8 *text)
{
	UINT8 i;
	UINT16 hoff;
	
	if(gStampVideoMode == 1) {
		// --- 录像模式：使用原版清晰字库，绝不拉丝 ---
		for(i=0; i<20 && text[i]; i++, xOff+=stampFontWidth){ 
			hoff = stampFontHOffsetGet(text[i]);
			HAL_GprmCopy(stampFontAddr
				, (UINT16)stampFontWidth* (UINT16)stampFontNum, (UINT16)stampFontHeight
				, hoff, 0
				, addrW
				, w, h
				, xOff, yOff
				, (UINT16)stampFontWidth, (UINT16)stampFontHeight
				, pasteThd
				, 0);  
		}
	} else {
		// --- 拍照模式：维持你开发的美观的点阵 ---
		stampAutoScale(w, h);
		stampDotInit();
		for(i=0; i<20 && text[i]; i++) {
			if(text[i]>='0' && text[i]<='9') {
				stampPaintPattern(addrW, w, h, xOff, yOff, pasteThd, digitPatterns[text[i]-'0']);
				xOff += (DOT_COLS*(dotSize+dotSpace))+dotSpace*2;
			}
			else if(text[i]=='/') {
				stampPaintPattern(addrW, w, h, xOff, yOff, pasteThd, slashPattern);
				xOff += (DOT_COLS*(dotSize+dotSpace))+dotSpace*2;
			}
			else if(text[i]==':') {
				stampPaintPattern(addrW, w, h, xOff, yOff, pasteThd, colonPattern);
				xOff += (DOT_COLS*(dotSize+dotSpace))+dotSpace*2; 
			}
			else if(text[i]=='.') {
				stampPaintPattern(addrW, w, h, xOff, yOff, pasteThd, dotPattern);
				xOff += (DOT_COLS*(dotSize+dotSpace))+dotSpace*2;
			}
			else if(text[i]==' ') {
				xOff += (DOT_COLS*(dotSize+dotSpace))+dotSpace*4;
			}
			xOff = (xOff + 1) & ~1; 
		}
	}
}

static void stampDrawDot(UINT32 addrW,UINT16 w,UINT16 h,UINT16 x,UINT16 y,UINT8 pasteThd)
{
	x = x & ~1;
	HAL_GprmCopy((UINT32)dotBuffer >> 1, dotSize, dotSize, 0, 0, addrW, w, h, x, y, dotSize, dotSize, pasteThd, 0);
}

static void stampPaintPattern(UINT32 addrW, UINT16 w, UINT16 h, UINT16 x, UINT16 y, UINT8 pasteThd, const UINT8 *pattern)
{
    UINT8 r, c;
    UINT16 xBase, yPos, xPos;

    xBase = x & ~1;

    for (r = 0; r < DOT_ROWS; r++) {
        for (c = 0; c < DOT_COLS; c++) {
            if (pattern[r] & (0x10 >> c)) {
                xPos = xBase + (c * (dotSize + dotSpace));
                xPos = xPos & ~1;  
                yPos = y + (r * (dotSize + dotSpace));
                
                // 确保坐标和大小在有效范围内
                if(xPos >= w || yPos >= h || xPos + dotSize > w || yPos + dotSize > h) continue;
                if(dotSize == 0) continue;

                HAL_GprmCopy((UINT32)dotBuffer >> 1, dotSize, dotSize, 0, 0, addrW, w, h, xPos, yPos, dotSize, dotSize, pasteThd, 0);
            }
        }
    }
}

void stampPaintTextToBackground(UINT8* text,UINT32 xOff,UINT32 yOff,UINT8 pasteThd)
{
	stampPaintText(stampBGAddr, stampBGWidth, stampBGHeight, xOff, yOff, pasteThd, text);
}

void stampBackgroundGet(UINT32* addrW,UINT32* xSize,UINT32* ySize)
{
	*addrW = stampBGAddr;
	*xSize = stampBGWidth;
	*ySize = stampBGHeight;
}

void stampBackgroundSet(UINT32 addrW,UINT16 xSize,UINT16 ySize)
{
	stampBGAddr = addrW;
	stampBGWidth = xSize;
	stampBGHeight = ySize;
}

UINT8 stampBackgroundEnable(UINT8 enable)
{
	if(enable!=0xff){
		stampBGEnable = enable;
	}
	return stampBGEnable;
}

UINT8 stampTextEnable(stamp_text_type type,UINT8 enable)
{
	stamp_text_t* ptText = dtText + type;
	if(enable!=0xff){		
		ptText->enable = enable;
	}
	return ptText->enable;
}

void stampTextSet(stamp_text_type type,UINT32 xOff,UINT32 yOff,UINT8 pasteThd)
{
	stamp_text_t* ptText = dtText + type;
	ptText->xOff = (xOff>stampBGWidth?stampBGWidth:xOff) & ~1;
	ptText->yOff = yOff>stampBGHeight?stampBGHeight:yOff;;
	ptText->pasteThd = pasteThd;
}

stamp_text_t* stampTextGet(stamp_text_type type)
{
	return dtText + type;	
}

void stampTextImageSet(stamp_text_type type, UINT32 imgAddr, UINT32 size)
{
    stamp_text_t* ptText = dtText + type;	
    UINT16 height = stampFontHeight;
    UINT16 width = size / 2 / stampFontWidth;
    
    ptText->imgAddrW = imgAddr;
    ptText->imgSize = size;
    
    if(width + ptText->xOff > stampBGWidth) { width = stampBGWidth - ptText->xOff; }
    if(height + ptText->yOff > stampBGHeight) { height = stampBGHeight - ptText->yOff; }
    
    ptText->imgWidth = width & 0xfff8;
    ptText->imgHeight = height;

    if (imgAddr && size) {
        // --- 核心修复：填充 0 而不是 0x80，避免大块灰阶 ---
        HAL_GprmDramFill(imgAddr, size, 0); 
    }
}

// 缓存字符串逻辑：隔离录像和拍照逻辑以防冲突
void stampTextModifyByString(stamp_text_type type,UINT8* str)
{
	stamp_text_t* ptText = dtText + type;
	UINT8* text = ptText->text;
	UINT8 c = strlen(str);
	
	// 因为录像没有分配 imgAddrW，它压根进不来这个 if，直接绕过了花屏逻辑
	if(gStampVideoMode == 0 && ptText->imgAddrW){
		UINT32 addrW = ptText->imgAddrW;
		UINT16 w = c * stampFontWidth;
		UINT16 h = stampFontHeight;
		
		if(w + ptText->xOff <= stampBGWidth){
			ptText->imgWidth = w;
			stampPaintText(addrW, w, h, 0, 0, ptText->pasteThd, str);
		}
	}
	memcpy(text, str, c + 1);
}
// 获取并更新日期时间：隔离显示行数逻辑以防录像水印变单行
void stampTextModifyByRTC(dateStc_t* rtc)
{
	stamp_text_t* ptDate = dtText + STAMP_DATE;
	stamp_text_t* ptTime = dtText + STAMP_TIME;
	UINT8 text[40];
	
	if(!rtc){
		rtc = &stampDateInf;
		HAL_GlobalReadRTC(rtc);
	}else{
		stampDateInf = *rtc;
	}
	
	if(gStampVideoMode == 1) {
		// 录像：双行字库
		if(ptDate->enable){
			stampDateFormat(text, rtc->Year, rtc->Month, rtc->Day, stampDateFmt);
			stampTextModifyByString(STAMP_DATE, text);
		}
		if(ptTime->enable){
			stampTimeFormat(text, rtc->Hour, rtc->Minute, rtc->Second, stampTimeFmt);
			stampTextModifyByString(STAMP_TIME, text);
		}
	} else {
		// 拍照：单行点阵
		UINT8 dateStr[20];
		UINT8 timeStr[20];
		stampDateFormat(dateStr, rtc->Year, rtc->Month, rtc->Day, stampDateFmt);
		stampTimeFormat(timeStr, rtc->Hour, rtc->Minute, rtc->Second, stampTimeFmt);
		sprintf((char*)text,"%s %s",dateStr,timeStr);
		
		if(ptDate->enable){
			stampTextModifyByString(STAMP_DATE, text);
		}
		ptTime->enable = 0;
	}
}

void stampTextModifyByImage(stamp_text_type type,UINT32 imgAddrW,UINT16 imgw,UINT16 imgh)
{
	stamp_text_t* ptText = dtText + type;
	if(ptText->imgAddrW && ptText->imgSize>(UINT32)imgw*imgh*2){
		if(imgw && imgh){
			ptText->imgWidth = imgw;
			ptText->imgHeight = imgh;
		}
		HAL_GprmDramDmaAdv(
			imgAddrW<<1
			, ptText->imgAddrW<<1
			, (UINT32)ptText->imgWidth*ptText->imgHeight*2
			, 0);
	}
}

void stampTextModifyByGTimer(UINT32 gTimer)
{
	UINT32 dTimer;

	if(gTimer==0){
		HAL_GlobalReadGTimer(&gTimer);
	}
	if(gTimer<=stampGTimer)return;

	dTimer= gTimer - stampGTimer;

	if(dTimer<1000)return;

	dTimer/=1000;

	stampDateTimeUpdate(&stampDateInf, dTimer);

	stampTextModifyByRTC(&stampDateInf);

	stampGTimer+=dTimer*1000;
}


void stampCombine(UINT32 dstAddrW)
{
	UINT8 i;
	// 必须拷底框防噪点
	if(stampBGAddr && stampBGEnable && stampBGAddr != dstAddrW){
		HAL_GprmDramDmaAdv(stampBGAddr << 1, dstAddrW << 1, (UINT32)stampBGWidth * stampBGHeight * 2, 1);
	}
	
	for(i = 0; i < STAMP_TYPE_MAX; i++)
	{
		stamp_text_t* ptText = dtText + i;
		if(!ptText->enable) continue;
		
		if(ptText->imgAddrW && gStampVideoMode == 0){
			HAL_GprmCopy(
				ptText->imgAddrW
				, ptText->imgWidth, ptText->imgHeight, 0, 0
				, dstAddrW
				, stampBGWidth, stampBGWidth, ptText->xOff, ptText->yOff
				, ptText->imgWidth, ptText->imgHeight
				, ptText->pasteThd
				, 0
			);
		} else {
			// 录像因为没缓存，一定会走到这里，执行全量大图重绘
			stampPaintText(dstAddrW, stampBGWidth, stampBGHeight, ptText->xOff, ptText->yOff, ptText->pasteThd, ptText->text);
		}
	}
}

void stampCombineNext(UINT32 dstAddrW)
{
	stampVideoModeSet(1); 
	stampDateTimeNext(&stampDateInf);
	stampTextModifyByRTC(&stampDateInf);
	stampCombine(dstAddrW);	
}

void stampCombineToImage(UINT32 dstAddrW,UINT16 w,UINT16 h,UINT16 xoff,UINT16 yoff,UINT8 pasteThr)
{	
	UINT8 i;
	if(stampBGAddr && stampBGEnable && stampBGAddr!=dstAddrW){
		HAL_GprmCopy(
			stampBGAddr
			, stampBGWidth, stampBGHeight, 0, 0
			, dstAddrW
			, w, h, xoff, yoff
			, stampBGWidth, stampBGHeight
			, pasteThr
			, 0
			);
	}
	for(i=0; i<STAMP_TYPE_MAX; i++)
	{
		stamp_text_t* ptText = dtText+i;
		if(!ptText->enable)continue;
		
		if(ptText->imgAddrW && gStampVideoMode == 0){
			HAL_GprmCopy(
				ptText->imgAddrW
				, ptText->imgWidth, ptText->imgHeight, 0, 0
				, dstAddrW
				, w, h, xoff+ptText->xOff, yoff+ptText->yOff
				, ptText->imgWidth, ptText->imgHeight
				, ptText->pasteThd
				, 0
			);
		}else{
			stampPaintText(
				dstAddrW
				, w, h
				, xoff+ptText->xOff, yoff+ptText->yOff
				, ptText->pasteThd
				, ptText->text
			);
		}
	}
}

void stampDateTimeUpdate(dateStc_t* dateInfNext,UINT32 skipSecond)
{
	UINT8 day=31;
	if(dateInfNext==NULL){
		dateInfNext = &stampDateInf;
	}	
	if(skipSecond==0){
		return;
	}else if(skipSecond==0xffffffff){
		HAL_GlobalReadRTC(dateInfNext);
	}
	dateInfNext->Second+=skipSecond;
	if(dateInfNext->Second<60)return;
	dateInfNext->Minute+=dateInfNext->Second/60;
	dateInfNext->Second=dateInfNext->Second%60;
	if(dateInfNext->Minute<60)return;
	dateInfNext->Hour+=dateInfNext->Minute/60;
	dateInfNext->Minute=dateInfNext->Minute%60;
	if(dateInfNext->Hour<24)return;
	dateInfNext->Day+=dateInfNext->Hour/24;
	dateInfNext->Hour=dateInfNext->Hour%24;

	while(1){
		if((dateInfNext->Month==4) || (dateInfNext->Month==6) || (dateInfNext->Month==9) || (dateInfNext->Month==11)){
			day = 30;
		}
		if(dateInfNext->Month==2){
			if((dateInfNext->Year%400==0)||((dateInfNext->Year%4==0)&&(dateInfNext->Year%100!=0))){
				day=29;
			}else{
				day=28;
			}
		}
		if(dateInfNext->Day<day)
		{	
			dateInfNext->Year += dateInfNext->Month/12;
			break;
		}
		dateInfNext->Month++;
		dateInfNext->Day-=day;	
	}
}

void stampDateTimeNext(dateStc_t* dateInfNext)
{
#if 1
	UINT8 day=31;
	if(dateInfNext==NULL){
		dateInfNext = &stampDateInf;
	}
	dateInfNext->Second++;
	if(dateInfNext->Second<60)return;
	dateInfNext->Minute++;
	dateInfNext->Second=0;
	if(dateInfNext->Minute<60)return;
	dateInfNext->Hour++;
	dateInfNext->Minute=0;
	if(dateInfNext->Hour<24)return;
	dateInfNext->Day++;
	dateInfNext->Hour=0;
	
	if((dateInfNext->Month==4) || (dateInfNext->Month==6) || (dateInfNext->Month==9) || (dateInfNext->Month==11)){
		day = 30;
	}
	if(dateInfNext->Month==2){
		if((dateInfNext->Year%400==0)||((dateInfNext->Year%4==0)&&(dateInfNext->Year%100!=0))){
			day=29;
		}else{
			day=28;
		}
	}
	if(dateInfNext->Day<=day)return;
	dateInfNext->Month++;
	dateInfNext->Day = 1;
	if(dateInfNext->Month<=12)return;
	dateInfNext->Year++;
	dateInfNext->Month = 1;

#endif
}

dateStc_t* stampRTCInfoGet()
{
	return &stampDateInf;
}

UINT32 stampGTimerGet()
{
	return stampGTimer;
}

void stampVideoModeInit(UINT32 fontAddr, UINT16 fontWidth, UINT16 fontHeight, UINT32 stampAddr, 
				 UINT32 imgAddr, UINT16 stampW, UINT16 stampH, UINT32 tmpAddr)
{
	stampVideoModeSet(1); 
	
	stampFontSet(fontAddr, fontWidth, fontHeight, tmpAddr);
	stampBackgroundSet(imgAddr, stampW, stampH);
	stampBackgroundEnable(1);
	stampTextReset();
	stampDateTimeFmtSet(0, 2);
	
	// 恢复双行显示，自动计算第二行位置防止重叠
	stampTextSet(STAMP_DATE, 0, 0, 5);
	stampTextSet(STAMP_TIME, 0, stampH / 2, 5); 
	
	// 绝对不要在这里调用 stampTextImageSet ！！！
	
	stampTextEnable(STAMP_DATE, 1);
	stampTextEnable(STAMP_TIME, 1);
	
	stampTextModifyByRTC(NULL);
	stampCombine(stampAddr);
}

#endif