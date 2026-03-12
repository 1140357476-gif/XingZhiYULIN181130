/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2007 by Sunplus mMedia Inc.                      *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus        *
 *  mMedia Inc. All rights are reserved by Sunplus mMedia Inc.  This      *
 *  software may only be used in accordance with the corresponding        *
 *  license agreement. Any unauthorized use, duplication, distribution,   *
 *  or disclosure of this software is expressly forbidden.                *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus mMedia Inc.                                *
 *                                                                        *
 *  Sunplus mMedia Inc. reserves the right to modify this software        *
 *  without notice.                                                       *
 *                                                                        *
 *  Sunplus mMedia Inc.                                                   *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *  Author: sunyong                                                *
 *                                                                        *
 **************************************************************************/
/**
 * @file		datestamp.c
 * @brief		sp1k API for RTC init.
 * @author	sun yong
 * @since		2008-03-01
 * @date		2008-03-01
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
//=============================================================================
//Symbol
//=============================================================================
//-----------------------------------------------------------------------------
//Constant
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//Variable
//-----------------------------------------------------------------------------
#if STAMP_EN
#define __FILE	__FILE_ID_DATESTAMP__

stamp_font_t stampFonts[STAMP_FONT_RES_NUM];

UINT32 stampFontAddr;
UINT16 stampFontWidth,stampFontHeight;
UINT8 stampFontNum;
UINT32 stampBGAddr;
//UINT16 stampBGXOff,stampBGYOff;
UINT16 stampBGWidth,stampBGHeight;
UINT32 stampImgAddr;
//UINT16 stampImgWidth,stampImgHeight;
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

/**
 * @brief		load date stamp yuv for nand
 * @param	smallSize : 0 big size, 1 normal size , 2 small size
 * @param	tmpAddr  : if smallsize is 1 or 2,the tmpaddress must be used when scaling
 * @retval	NONE
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @bug		NONE.
*/
UINT8 stampFontResLoad(UINT8 id,UINT8 resId,UINT8 fontW,UINT8 fontH,UINT8 fontNum)
{
	stamp_font_t *pFont = stampFonts+id-1;
	UINT32 addr = K_SDRAM_GrafFontBufAddr;
	//printf("%bu-%bu,%bu,%bu,%bu,%bu\n", id,(UINT8)STAMP_FONT_RES_NUM, resId, fontW, fontH, fontNum);
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
	//memcpy(stampFonts,pFont,sizeof(stampFonts));
		
	return ret;
}
stamp_font_t* stampFontSet(UINT32 fontAddr,UINT16 fontWidth,UINT16 fontHeight,UINT32 tmpAddr)
{
	UINT8 i;//,idx;
	stamp_font_t *pFont = stampFonts;
	//UINT16 w=DATE_STAMP_H_SIZE_7,h=DATE_STAMP_V_SIZE_7;
	stampFontAddr = fontAddr;
	stampFontWidth = fontWidth;
	stampFontHeight = fontHeight;	
	/*{
		stamp_font_t *pf = stampFonts;
	for(i=0;i<STAMP_FONT_RES_NUM;i++,pf++)
	{
		printf("%bu-(%bu,%bu,%bu)\n",i,pf->width,pf->height,pf->count);
	}
		}
	*/
	for(i=0;i<7;i++,pFont++){
		//printf("%bu-(%bu,%bu,%bu)(%u,%u)\n",i,pFont->width,pFont->height,pFont->count,fontWidth, fontHeight);
		if(fontWidth >= pFont->width && fontHeight >= pFont->height){
			//w=pFont->width;
			//h=pFont->height;
			break;
		}
	}
	//printf("%bu-(%bu,%bu,%bu)(%u,%u)\n",i,pFont->width,pFont->height,pFont->count,fontWidth, fontHeight);

	if(i>=7){i=6;pFont--;}
	
	stampFontNum = pFont->count;
	if(fontAddr && tmpAddr){
		HAL_GprmScale(pFont->addr//*(paddrs[i])
			, (UINT16)pFont->width* (UINT16)stampFontNum, (UINT16)pFont->height,
			fontAddr
			, (UINT16)fontWidth * (UINT16)stampFontNum 
			, (UINT16)fontHeight, tmpAddr
			, 0);
		//printf("%bu-(%bu,%bu,%bu)(%u,%u)\n",i,pFont->width,pFont->height,pFont->count,fontWidth, fontHeight);
		//dramFileSave("FF0.YUV",pFont->addr, (UINT32)pFont->width* stampFontNum*pFont->height*2);
		//dramFileSave("FF1.YUV",fontAddr, fontWidth*fontHeight*stampFontNum*2);
	}
	return pFont;
}
#if 0
void stampFontSet(UINT32 fontAddr,UINT16 fontWidth,UINT16 fontHeight,UINT32 tmpAddr)
{
	code UINT8 sizes[] = {
		 DATE_STAMP_H_SIZE_1,DATE_STAMP_V_SIZE_1
		,DATE_STAMP_H_SIZE_2,DATE_STAMP_V_SIZE_2
		,DATE_STAMP_H_SIZE_3,DATE_STAMP_V_SIZE_3
		,DATE_STAMP_H_SIZE_4,DATE_STAMP_V_SIZE_4
		,DATE_STAMP_H_SIZE_5,DATE_STAMP_V_SIZE_5
		,DATE_STAMP_H_SIZE_6,DATE_STAMP_V_SIZE_6
		,DATE_STAMP_H_SIZE_7,DATE_STAMP_V_SIZE_7		
	};
	code UINT32 xdata* paddrs[] = {&dsAddr1,&dsAddr2,&dsAddr3,&dsAddr4,&dsAddr5,&dsAddr6,&dsAddr7};
	UINT8 i,idx;
	UINT16 w=DATE_STAMP_H_SIZE_7,h=DATE_STAMP_V_SIZE_7;
	stampFontAddr = fontAddr;
	stampFontWidth = fontWidth;
	stampFontHeight = fontHeight;	
	for(i=0;i<7;i++){
		idx= i<<1;
		if(fontWidth >= sizes[idx] && fontHeight >= sizes[idx+1]){
			w=sizes[idx];
			h=sizes[idx+1];
			break;
		}
	}
	//printf("%bu-%u,%u\n",i,w,h);
	HAL_GprmScale(*(paddrs[i])
		, (UINT16)w * (UINT16)stampFontNum, (UINT16)h,
		fontAddr
		, (UINT16)fontWidth * (UINT16)stampFontNum 
		, (UINT16)fontHeight, tmpAddr
		, 0);

}
#endif
/*
 * set date & time view format
 * @param   mode  : dateFmt: 0: yyyy/mm/dd
 *\n 						 1: dd/mm/yyyy
 *\n 						 2: mm/dd/yyyy
 *\n						 3: yy/mm/dd
 *\n 			    timeFmt:   
*\n 						0: hh:mm
*\n 						1: hh:mm AM/PM
*\n 						2: hh:mm:ss
*\n 						3: hh:mm:ss AM/PM
*\n 						4: hh:mm:ss A/P
*\n 						5: hh:mm A/P
*/
void stampDateTimeFmtSet(UINT8 dateFmt,UINT8 timeFmt)
{
	stampDateFmt = dateFmt;
	stampTimeFmt = timeFmt;
}
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
void stampTimeFormat(UINT8* text,UINT8 hour,UINT8 minute,UINT8 second,UINT8 fmt)
{
	UINT8 ai = 0;
	UINT8 pattern[]= "%02bu:%02bu:%02bu AM";
	//code UINT8 pattern1[] = "%02bu:%02bu%cM";
	
	
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
/*
* create Date string 
* @param   mode  : fmt
*\n 0: yyyy/mm/dd
*\n 1: dd/mm/yyyy
*\n 2: mm/dd/yyyy
*\n 3: yy/mm/dd
*/
void stampDateFormat(UINT8* text,UINT8 year,UINT8 mon,UINT8 day,UINT8 fmt)
{
	//dateStc_t dateInf;
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
		val = val - '/'; //cont. chars
	}
	else {//xian ++ for date stamp string
		switch (val){                    
		/*
		case 'C':
			val = ':' - '/' + 1;
			break;

		case 'F':
			val = ':' - '/' + 2;
			break;
		*/    
		case 'A':
			val = ':' - '/' + 1;
			break;
		case 'P':
			val = ':' - '/' + 2;
			break;
		case 'M':
			val = ':' - '/' + 3;
			break;                    	
		default:                    
			val = stampFontNum-1;   /*undefine char , it's space normally */           
			break;
		}
	}
	hoff = (UINT16)val * stampFontWidth;
	return hoff;
}
//paint text to image at(xoff,yoff)
void stampPaintText(UINT32 addrW,UINT16 w,UINT16 h,UINT16 xOff,UINT16 yOff,UINT8 pasteThd,UINT8 *text)
{
	UINT8 i,chr=text[0];
	UINT16 hoff;
	//while(chr && i++<20){
	//printf("%lx(%u,%u)(%u,%u),%lx(%u,%u)%bu\n",addrW, w, h, xOff, yOff
	//	,stampFontAddr	, stampFontWidth, stampFontHeight,stampFontNum);
	for(i=0;  i<20 && text[i];  i++,xOff+=stampFontWidth){
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
		//chr=text[i];
		//xOff+=stampFontWidth;
		//printf("%bu,%c,%u--",i,text[i],hoff);
	}
}
//paint text to background image at(xoff,yof)
void stampPaintTextToBackground(UINT8* text,UINT32 xOff,UINT32 yOff,UINT8 pasteThd)
{
	stampPaintText(
		stampFontAddr
		,stampFontWidth,stampFontHeight, xOff, yOff
		,pasteThd
		,text
		);
}
//return stamp background image addr/width/height
void stampBackgroundGet(UINT32* addrW,UINT32* xSize,UINT32* ySize)
{
	*addrW = stampBGAddr;
	*xSize = stampBGWidth;
	*ySize = stampBGHeight;
}
//set stamp background image addr/width/height
void stampBackgroundSet(UINT32 addrW,UINT16 xSize,UINT16 ySize)
{
	stampBGAddr = addrW;
	stampBGWidth = xSize;
	stampBGHeight = ySize;
}
/*
void stampImageSet(UINT32 addrW,UINT16 xSize,UINT16 ySize,UINT16 xOff,UINT16 yOff)
{
	stampImgAddr = addrW;
	stampImgWidth = xSize;
	stampImgHeight = ySize;
	stampBGXOff = xOff;
	stampBGYOff = yOff;
}*/
//set switch, that copy background image to stamp or not,when combine
UINT8 stampBackgroundEnable(UINT8 enable)
{
	if(enable!=0xff){
		stampBGEnable = enable;
	}
	return stampBGEnable;
}
//set switch, that paint text selected to stamp or not,when combine
UINT8 stampTextEnable(stamp_text_type type,UINT8 enable)
{
	stamp_text_t* ptText = dtText + type;
	if(enable!=0xff){		
		ptText->enable = enable;
	}
	return ptText->enable;
}
/*set text property
* @param  : 
* xoff,yoff: text painting postion
* pasteThd: 
*/
void stampTextSet(stamp_text_type type,UINT32 xOff,UINT32 yOff,UINT8 pasteThd)
{
	stamp_text_t* ptText = dtText + type;
	ptText->xOff = xOff>stampBGWidth?stampBGWidth:xOff;
	ptText->yOff = yOff>stampBGHeight?stampBGHeight:yOff;;
	ptText->pasteThd = pasteThd;
}
stamp_text_t* stampTextGet(stamp_text_type type)
{
	return 	 dtText + type;	
}
/*set image of text property
* @param  : 
* imgAddr:	 image of text 
*  width, height: 
*/
void stampTextImageSet(stamp_text_type type,UINT32 imgAddr,UINT32 size)//INT16 width,UINT16 height)
{
	stamp_text_t* ptText = dtText + type;	
	UINT16 height = stampFontHeight;
	UINT16 width = size/2/stampFontWidth;
	ptText->imgAddrW = imgAddr;
	ptText->imgSize = size;
	
	//if(!width){width = (UINT16)stampFontWidth*sizeof(ptText->text);}
	if(width+ptText->xOff>stampBGWidth) 	{width = stampBGWidth-ptText->xOff;	}
	//if(!height){height = stampFontHeight;}
	if(height+ptText->yOff>stampBGHeight) 	{height = stampBGHeight-ptText->yOff;	}
	ptText->imgWidth = width & 0xfff8;
	ptText->imgHeight= height;// & 0xfff8;
	//ptText->imgSize = (UINT32)ptText->imgWidth*ptText->imgHeight;
}

//extern char putchar(char c);
/*modify text using string, update differnce only
*/
void stampTextModifyByString(stamp_text_type type,UINT8* str)
{
	stamp_text_t* ptText = dtText + type;
	UINT8* text = ptText->text;
	UINT8 i,c=strlen(str);
	if(ptText->imgAddrW){
		UINT32 addrW = ptText->imgAddrW;
		UINT16 w =c*stampFontWidth;
		UINT16 h = stampFontHeight;
		UINT16 hoff;
		//if(/*((UINT32)w*h<=ptText->imgSize) &&*/ (w+ptText->xOff<=stampBGWidth) /*&&(h+ptText->yOff<=stampBGHeight)*/){
			//ptText->imgWidth = w;
			//ptText->imgHeight = h;
		if(w+ptText->xOff<=stampBGWidth){
			ptText->imgWidth = w;
			for(i=0;i<c;i++)
			{
				if(text[i]!=str[i] && text[i] && str[i]){
					//putchar(str[i]);
					//text[i]=str[i];
					//fill black				
					//copy font
					hoff = stampFontHOffsetGet(str[i]);
					HAL_GprmCopy(stampFontAddr
						, (UINT16)stampFontWidth* (UINT16)stampFontNum, (UINT16)stampFontHeight
						, hoff, 0
						, addrW
						, w, h	, stampFontWidth*i, 0
						, (UINT16)stampFontWidth, (UINT16)stampFontHeight
						, ptText->pasteThd
						, 0
						);  
				}
			}
		}
	}
	memcpy(text,str,c+1);
}
/*modify image of text
*/
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

/*modify date & time by gtimer
* param: gtimer==0, use current gtimer
*/
void stampTextModifyByGTimer(UINT32 gTimer)
{
	stamp_text_t* ptDate = dtText + STAMP_DATE;
	stamp_text_t* ptTime = dtText + STAMP_TIME;
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

/*modify date & time by RTC
* param: rtc==0, use current RTC
*/
void stampTextModifyByRTC(dateStc_t* rtc)
{
	stamp_text_t* ptDate = dtText + STAMP_DATE;
	stamp_text_t* ptTime = dtText + STAMP_TIME;
	UINT8 text[20];
	if(!rtc){
		rtc = &stampDateInf;
		HAL_GlobalReadRTC(rtc);
	}else{
		stampDateInf = *rtc;
	}
	if(ptDate->enable){
		stampDateFormat(text, rtc->Year, rtc->Month, rtc->Day, stampDateFmt);
		stampTextModifyByString(STAMP_DATE, text);
	}
	if(ptTime->enable){
		stampTimeFormat(text, rtc->Hour, rtc->Minute, rtc->Second, stampTimeFmt);
		stampTextModifyByString(STAMP_TIME, text);
		//printf("%bu-%s\n",stampTimeFmt,text);
	}	
}

/*combine background and image of text to dstImage
* param: dstAddrW
*/
void stampCombine(UINT32 dstAddrW)
{
	//stamp_text_t* ptText = dtText;
	UINT8 i;
	if(stampBGAddr && stampBGEnable && stampBGAddr!=dstAddrW){
		HAL_GprmDramDmaAdv(stampBGAddr<<1, dstAddrW<<1, (UINT32)stampBGWidth*stampBGHeight*2, 1);
		//printf("%lx,%lx,%u,%u\n",stampBGAddr,dstAddrW,stampBGWidth,stampBGHeight);
	}
	for(i=0;i<STAMP_TYPE_MAX;i++)
	{
		stamp_text_t* ptText = dtText+i;
		if(!ptText->enable){continue;}
		if(ptText->imgAddrW){
			HAL_GprmCopy(
				ptText->imgAddrW
				, ptText->imgWidth, ptText->imgHeight, 0, 0
				, dstAddrW
				, stampBGWidth, stampBGWidth, ptText->xOff, ptText->yOff
				, ptText->imgWidth, ptText->imgHeight
				, ptText->pasteThd
				, 0
				);
		}else{
			stampPaintText(
				dstAddrW
				, stampBGWidth, stampBGHeight
				, ptText->xOff, ptText->yOff
				, ptText->pasteThd
				, ptText->text
				);
		}
		//printf("%bu,%x,%x,%s\n",i,ptText->xOff,ptText->yOff,ptText->text);
	}
}

/*update date& time by RTC,combine background and image of text to dstImage
* param: dstAddrW
*/
void stampCombineNext(UINT32 dstAddrW)
{
	stampDateTimeNext(&stampDateInf);
	stampTextModifyByRTC(&stampDateInf);
	stampCombine(dstAddrW);	
}

/*combine background and image of text to dstImage at (xoff,yoff)
* param: dstAddrW,w,h,xoff,yoff
*/
void stampCombineToImage(UINT32 dstAddrW,UINT16 w,UINT16 h,UINT16 xoff,UINT16 yoff,UINT8 pasteThr)
{	
	UINT8 i;
	/*if(dstAddrW==0){
		dstAddrW = stampImgAddr;
	}
	if(w==0 || h==0){
		w = stampImgWidth;
		h = stampImgHeight;
		xoff = stampBGXOff;
		yoff = stampBGYOff;
	}*/
	#if 1
	if(stampBGAddr&& stampBGEnable && stampBGAddr!=dstAddrW){
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
	for(i=0;i<STAMP_TYPE_MAX;i++)
	{
		stamp_text_t* ptText = dtText+i;
		if(!ptText->enable)continue;
		if(ptText->imgAddrW){
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
	#endif
	//return TRUE;
}


/*update date& time by skipSencond
* param: 
*/
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
		if((dateInfNext->Month==4)
			||(dateInfNext->Month==6) 
			||(dateInfNext->Month==9)
			||(dateInfNext->Month==11)
			){
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
	
	//printf("*%bu\n",dateInfNext->Second);
}

/*update date& time by 1 skipSencond
* param: 
*/
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
	
	if((dateInfNext->Month==4)
		||(dateInfNext->Month==6) 
		||(dateInfNext->Month==9)
		||(dateInfNext->Month==11)
		){
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

#else
	dateInfNext->Second++;
	
	if(dateInfNext->Second>=60){
	 	dateInfNext->Second=0;
		dateInfNext->Minute++;
	}
	if(dateInfNext->Minute>=60){
	 	dateInfNext->Minute=0;
		dateInfNext->Hour++;
	}
	if(dateInfNext->Hour>=24){
		dateInfNext->Hour=0;
	
		if(((dateInfNext->Month==4)||(dateInfNext->Month==6) ||(dateInfNext->Month==9)||(dateInfNext->Month==11))&&(dateInfNext->Day==30)){
	    	dateInfNext->Day=1;
			dateInfNext->Month++;
		
		}
		else if(((dateInfNext->Month==3)||(dateInfNext->Month==5)||(dateInfNext->Month==7)||(dateInfNext->Month==8)||(dateInfNext->Month==10)||(dateInfNext->Month==12))&&(dateInfNext->Day==31)){
			dateInfNext->Day=1;
			dateInfNext->Month++;	
		}
		if((dateInfNext->Year%400==0)||((dateInfNext->Year%4==0)&&(dateInfNext->Year%100!=0)))
		{
			if((dateInfNext->Month==2)&&(dateInfNext->Day==29)){
				dateInfNext->Day=1;
				dateInfNext->Month++;
			}
		}
		else{
			if((dateInfNext->Month==2)&&(dateInfNext->Day==28)){
				dateInfNext->Day=1;
				dateInfNext->Month++;
			}
		}
	}
	if(dateInfNext->Month>=13)
	{
	  dateInfNext->Month=1;
	  dateInfNext->Year++;
	}
#endif
	//printf("*%bu\n",dateInfNext->Second);
}

dateStc_t* stampRTCInfoGet()
{
	return &stampDateInf;
}
UINT32 stampGTimerGet()
{
	return stampGTimer;
}
#if 0
void stampTestVideoInit()
{

	UINT32 addr = K_SDRAM_AviCaptureIdx1Addr + (UINT32)32*1024;
	UINT32 size = (UINT32)g_vidDateStamp.stampW*g_vidDateStamp.ftH*2;
	UINT 8 i;
	//Sp1kStampFontInit();
	
	//set font buf and w,h 
	stampFontSet(g_vidDateStamp.ftAddr,g_vidDateStamp.ftW,g_vidDateStamp.ftH,g_vidDateStamp.stampAddr);
	//set background buf and w,h
	stampBackgroundSet(g_vidDateStamp.imgAddr,g_vidDateStamp.stampW,g_vidDateStamp.stampH);
	// enable copy background image when combine
	stampBackgroundEnable(1);

	stampTextReset();
	// set date and time fomort
	stampDateTimeFmtSet(0,2);
	// set date postion(x,y) at background,and pasteThr
	stampTextSet(STAMP_DATE,  0,  0,  5);
	// set time postion(x,y) at background,and pasteThr
	stampTextSet(STAMP_TIME,   0,   48,  5);
	// set custom string postion(x,y) at background,and pasteThr
	stampTextSet(STAMP_TEXT,   30,   0,  5);

	// set date temp buf, speed up when updating
	stampTextImageSet(STAMP_DATE, addr, size);
	// set time temp buf, speed up when updating
	stampTextImageSet(STAMP_TIME, addr+size/2, size);
	// set time temp buf, speed up when updating
	stampTextImageSet(STAMP_TEXT, addr+size/2, size);

	// enable paint time when combine
	stampTextEnable(STAMP_DATE, 1);
	// enable paint date when combine
	stampTextEnable(STAMP_TIME, 1);	
	// enable paint custom when combine
	stampTextEnable(STAMP_TEXT, 1);

	// set custom string
	stampTextModifyByString(STAMP_TEXT,"1234567");

	//update date and time by current RTC
	stampTextModifyByRTC(NULL);

	//combine background/date/time/custom string to dstImage
	stampCombine(g_vidDateStamp.stampAddr);
	
	for(i=1;i<4;i++)
	{
		videoRecDateStampLoad(i);
	}

}
void stampTestVideoNext(UINT32 loadAddr)
{
	stampCombineNext(loadAddr);
}
void stampTest(UINT8* ptr)
{

}
#endif
#endif
