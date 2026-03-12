/**************************************************************************
 *		   Copyright(c) 2010 by i-Catch Technology Co., Ltd.			  *
 *																		  *
 *	This software is copyrighted by and is the property of i-Catch Tech-  *
 *	nology Co., Ltd. All rights are reserved by i-Catch Technology Co.,   *
 *	Ltd. This software may only be used in accordance with the			  *
 *	corresponding license agreement. Any unauthorized use, duplication,   *
 *	distribution, or disclosure of this software is expressly forbidden.  *
 *																		  *
 *	This Copyright notice "M U S T" not be removed or modified without	  *
 *	prior written consent of i-Catch Technology Co., Ltd.				  *
 *																		  *
 *	i-Catch Technology Co., Ltd. reserves the right to modify this		  *
 *	software without notice.											  *
 *																		  *
 *	i-Catch Technology Co., Ltd.										  *
 *	19-1, Innovation First Road, Science-Based Industrial Park, 		  *
 *	Hsin-Chu, Taiwan, R.O.C.											  *
 *																		  *
 *************************************************************************/

/**
 * @file	  video_dtstmp.c
 * @brief	  date stamp for video record
 * @author	  tq.zhang
 * @since	  2010-05-13
 * @date	  2010-05-13
*/

/**************************************************************************
 *						  H E A D E R	F I L E S
 **************************************************************************/
#include "video_def.h"
#include "video_cap_mem.h"
/**************************************************************************
 *							 C O N S T A N T S							  *
 **************************************************************************/

/**************************************************************************
 *								M A C R O S 							  *
 **************************************************************************/
 #define __FILE	  __FILE_ID_VIDEO_DTSTMP__

/**************************************************************************
 *							D A T A    T Y P E S						  *
 **************************************************************************/

/**************************************************************************
 *						   G L O B A L	  D A T A						  *
 **************************************************************************/
 UINT8 videoDtFlag[4];
xdata videoStampCfg_t g_vidDateStamp = {0};
UINT8 g_bReserveStampFrameSize = 0;
UINT8 vidDateStampSts = 0;	  //0: disable , 1: enable
//UINT8 vidDateStampEof = 0;
UINT8 vidRTCCount,vidRTCCount0;
dateStc_t videoDateInf;
extern UINT8 g_SlowSnapOpen;
/**************************************************************************
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/


UINT8 vidOsdTexts[2][4][20];
video_osd_text_t vidOsdText[2];
/**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/
 //void videoRecStampAligSet();
 //UINT8 videoRecDateStampInit(UINT16 imgW, UINT16 imgH);
// UINT8 videoRecImgStampInit(UINT16 imgW, UINT16 imgH);
 //void videoRecDateStampLoad(UINT8 idx);
 //void videoTimeNext(pDateStc_t dateInfNext);
 /**
 * @fn void videoRecStampCfg(UINT8 mode, UINT8 aligFmt)
 * @brief	Configure video stamp
 * @param[in] mode	Secify video stamp mode. See VDStampMode_t, they are VD_STAMP_MODE_DATE, VD_STAMP_MODE_IMG.
 *\n				and VD_STAMP_MODE_IMG_DATE.
 * @param[in] aligFmt  Secify video stamp display position both in horizontal and vertical use logic OR.
 *\n				   Horizontal positions are VD_STAMP_LEFT, VD_STAMP_CENTER and VD_STAMP_TOP.
 *\n				   Vertical positions are VD_STAMP_TOP, VD_STAMP_VCENTER, VD_STAMP_BOTTOM.
 * @param[in] recMode  Secify video stamp recording mode. See VDStampRecMode_t, they are VD_STAMP_REC_MODE_ALL_FRAME,
 *\n				   VD_STAMP_REC_MODE_FST_FRAME, VD_STAMP_REC_MODE_END_FRAME, and
 *\n				   VD_STAMP_REC_MODE_FST_END_FRAME.
 * @retval none
 * @see
 */
void videoRecStampCfg(UINT8 mode, UINT8 aligFmt, UINT8 recMode)
{
	mode = (mode & VD_STAMP_MODE_MASK) | ((recMode<<4) & VD_STAMP_RECMODE_MASK);
	g_vidParam[VD_STAMP_ALIG_MODE] = ((UINT16)aligFmt << 8) | mode;
}

 /**
 * @fn UINT8 videoRecStampImgResLoad(UINT16 resFileId, UINT16 stampW, UINT16 stampH)
 * @brief	load image resource file for video stamp
 * @param[in] resFileId  Secify the video stamp's resource file id.
 *\n					 Fill black if resFikeId is zero.
 * @param[in] stampW  Secify stamp image width.
 * @param[in] stampH  Secify stamp image height.
 * @retval SUCCESS or FAIL
 * @see
 */
 #if 0
UINT8 videoRecStampImgResLoad(UINT16 resFileId, UINT16 stampW, UINT16 stampH)
{
	UINT8 ret = TRUE;
	UINT8 frameRate;
	if((UINT32)stampW * stampH >vidRecMemSizeGet(VID_MEM_IDX_VIDEO_REC_StampA)/*K_SDRAM_AviStampBufSize*/){
		ASSERT(0, 1);
		return FAIL;
	}


	aeStatusGet(SP1K_AE_FRAMERATE,&frameRate);

	g_vidDateStamp.stampW = stampW;
	g_vidDateStamp.stampH = stampH;

	if ((frameRate==SP1K_FRAME_RATE_60)||(frameRate==SP1K_FRAME_RATE_50)){
		//g_vidDateStamp.stampAddr = (UINT32)K_SDRAM_AviPvwBufAddrD_60FPS - (UINT32)K_SDRAM_AviStampBufTotalSize;	//A, B buffer
		g_vidDateStamp.stampAddr=vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_StampA);
	}
	else{
		//g_vidDateStamp.stampAddr = (UINT32)K_SDRAM_AviPvwBufAddrD - (UINT32)K_SDRAM_AviStampBufTotalSize;	//A, B buffer
		g_vidDateStamp.stampAddr=vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_StampA);
	}

	//g_vidDateStamp.ftAddr = g_vidDateStamp.stampAddr  + (K_SDRAM_AviStampBufSize * 4/*<<1*/); //fnt buffer
	g_vidDateStamp.ftAddr = vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_Font);
	//g_vidDateStamp.imgAddr = g_vidDateStamp.ftAddr + K_SDRAM_AviStampFontBufSize; //img buffer
	g_vidDateStamp.imgAddr=vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_IMG);
	//read res image
	gprmBlackFill(g_vidDateStamp.imgAddr, stampW, stampH);
	if(resFileId){
		sp1kNandRsvRead((UINT32)resFileId, (g_vidDateStamp.imgAddr<< 1));
	}

	return SUCCESS;
}
#endif
 /**
 * @fn void videoRecStampStsSet(UINT8 en)
 * @brief	Enable or disable video stamp
 * @param[in] en  Secify video stamp status. 1:enable, 0:disable.
 * @retval none
 * @see videoRecStampStsGet()
 */
void videoRecStampStsSet (UINT8 en)
{
	vidDateStampSts = en;
}

 /**
 * @fn void videoRecStampStsGet(void)
 * @brief	Get if the stamp status
 * @param  none
 * @retval Video stamp status. 1:enable, 0:disable.
 * @see videoRecStampStsSet()
 */
UINT8 videoRecStampStsGet(void)
{
	return vidDateStampSts;
}
#if 0
/**
 * @fn		  void videoRecStampDateFmtSet(UINT8 fmt)
 * @brief	  set stamp date format
 * @param	  [in] fmt
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
void videoRecStampDateFmtSet(UINT16 fmt)
{
	g_vidParam[VD_STAMP_DATATIME_FMT] = (UINT16)fmt;
}

/**
 * @fn		  UINT8 videoRecStampDateFmtGet(void)
 * @brief	  get stamp date format
 * @param	  NONE
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT16 videoRecStampDateFmtGet(void)
{
	return (UINT16)(g_vidParam[VD_STAMP_DATATIME_FMT] & 0xffff);
}
#endif
//stamp initial
/**
 * @fn		  void videoRecStampInit(void)
 * @brief	  stamp inital
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
void videoRecStampInit()
{

	UINT8 mode;
	UINT8 i;
	extern SINT32 audDataSize;
	UINT16 offx = g_vidParam[VD_STAMP_DISOFFX];
	UINT16 offy = g_vidParam[VD_STAMP_DISOFFY];
	UINT8 c = 16;
	if(offx<c){offx=c;}
	if(offy<c){offy=c;}
	if(offx>g_vidDateStamp.imgW -g_vidDateStamp.stampW-16){offx =g_vidDateStamp.imgW - g_vidDateStamp.stampW-16;}
	if(offy>g_vidDateStamp.imgH -g_vidDateStamp.stampH-16){offy =g_vidDateStamp.imgH - g_vidDateStamp.stampH-16;}
	g_vidDateStamp.disHOff = offx;
	g_vidDateStamp.disVOff = offy;
#if 0
	UINT8 frameRate;
	UINT32 addr = K_SDRAM_AviCaptureIdx1Addr + (UINT32)32*1024;
	UINT32 size;// = (UINT32)g_vidDateStamp.stampW*g_vidDateStamp.ftH*2;

	printf("v2\n");
	aeStatusGet(SP1K_AE_FRAMERATE,&frameRate);

	if ((frameRate==SP1K_FRAME_RATE_60)||(frameRate==SP1K_FRAME_RATE_50)){
		g_vidDateStamp.stampAddr = (UINT32)K_SDRAM_AviPvwBufAddrD_60FPS - (UINT32)K_SDRAM_AviStampBufTotalSize;	//A, B buffer
	}
	else{
		g_vidDateStamp.stampAddr = (UINT32)K_SDRAM_AviPvwBufAddrD - (UINT32)K_SDRAM_AviStampBufTotalSize;	//A, B buffer
	}

	g_vidDateStamp.ftAddr = g_vidDateStamp.stampAddr  + (K_SDRAM_AviStampBufSize * 4/*<<1*/); //fnt buffer
	g_vidDateStamp.imgAddr = g_vidDateStamp.ftAddr + K_SDRAM_AviStampFontBufSize; //img buffer

	/*//read res image
	if(resId){
		sp1kNandRsvRead((UINT32)resFileId, (g_vidDateStamp.imgAddr<< 1));
	}else{
		gprmBlackFill(g_vidDateStamp.imgAddr, stampW, stampH);
	}*/

	//set font buf and w,h
	stampFontSet(g_vidDateStamp.ftAddr,g_vidDateStamp.ftW,g_vidDateStamp.ftH,g_vidDateStamp.stampAddr);
	//set background buf and w,h
	stampBackgroundSet(g_vidDateStamp.imgAddr,g_vidDateStamp.stampW,g_vidDateStamp.stampH);
	// enable copy background image when combine
	stampBackgroundEnable(1);

	size = g_vidDateStamp.stampW*g_vidDateStamp.ftH*2;
	// set date temp buf, speed up when updating
	stampTextImageSet(STAMP_DATE, addr, size);
	// set time temp buf, speed up when updating
	stampTextImageSet(STAMP_TIME, addr+size/2, size);
	// set time temp buf, speed up when updating
	stampTextImageSet(STAMP_TEXT, addr+size/2, size);
#endif
	//update date and time by current RTC
	stampTextModifyByRTC(NULL);

	//combine background/date/time/custom string to dstImage
	stampCombine(g_vidDateStamp.stampAddr);
	vidOsdText[STAMP_DATE].text= vidOsdTexts[STAMP_DATE][0];
	vidOsdText[STAMP_TIME].text= vidOsdTexts[STAMP_TIME][0];
	strcpy(vidOsdText[STAMP_DATE].text,stampTextGet(STAMP_DATE)->text);
	strcpy(vidOsdText[STAMP_TIME].text,stampTextGet(STAMP_TIME)->text);
	for(i=1;i<4;i++)
	{
		videoRecStampLoad(i);
	}
	//videoRecStampAligSet();
	videoDtFlag[0]=0;
	audDataSize=audBytesPerSec;
	g_vidDateStamp.disBufId=0;

	mode = videoRecStampRecModeGet();
	switch(mode){
		case VD_STAMP_REC_MODE_END:
		case VD_STAMP_REC_MODE_FST_END_FRAME:
			g_bReserveStampFrameSize = 1;
			break;

		case VD_STAMP_REC_MODE_ALL_FRAME:
		case VD_STAMP_REC_MODE_FST_FRAME:
		default:
			g_bReserveStampFrameSize = 0;
			break;
	}
	vidRTCCount0 = 0;
	vidRTCCount = vidRTCCount0;
	HAL_GlobalRTCSecondIntrSet(1);
}

//update stamp
/**
 * @fn		  void videoRecStampLoad(void)
 * @brief	  load date stamp
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
void videoRecStampLoad(UINT8 idx)
{
	/*UINT8 mode;

	mode = videoRecStampModeGet();

	if(mode == VD_STAMP_MODE_DATE || mode == VD_STAMP_MODE_IMG_DATE){
		videoRecDateStampLoad(idx); //load A, B,Tmp buffer
	}*/
	UINT32 loadAddr=g_vidDateStamp.stampAddr+(K_SDRAM_AviStampBufSize*idx);

	stampCombineNext(loadAddr);
	videoDtFlag[idx]=1;

	memcpy(vidOsdTexts[STAMP_DATE][idx],stampTextGet(STAMP_DATE)->text,20);
	memcpy(vidOsdTexts[STAMP_TIME][idx],stampTextGet(STAMP_TIME)->text,20);

}
video_osd_text_t* videoRecOsdTextGet(stamp_text_type type)
{
	return vidOsdText+(UINT8)type;
}
#if 0
/**
 * @fn		  UINT8 videoRecStampModeGet(void)
 * @brief	  get stamp mode: all frame/first frame/end frame/first&end frame
 * @param	  NONE
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
static UINT8 videoRecStampModeGet(void)
{
	return ((UINT16)g_vidParam[VD_STAMP_ALIG_MODE] & VD_STAMP_MODE_MASK);
}
#endif
/**
 * @fn		  UINT8 videoRecStampRecModeGet(void)
 * @brief	  get record mode : all frame/first frame/end frame/first&end frame
 * @param	  NONE
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 videoRecStampRecModeGet(void)
{
	return ((UINT16)g_vidParam[VD_STAMP_ALIG_MODE] & VD_STAMP_RECMODE_MASK) >> 4;
}
#if 0
/**
 * @fn		  UINT8 videoRecStampAligFmtGet(void)
 * @brief	  stamp align mode: left/center/right top/center/bottom
 * @param	  NONE
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 videoRecStampAligFmtGet()
{
	return (UINT8)((g_vidParam[VD_STAMP_ALIG_MODE] & 0xff00) >> 8);
}
#endif
/**
 * @fn		  void videoRecStampAligSet(void)
 * @brief	  video Record Stamp Align Set
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-10
 * @todo	  N/A
 * @bug 	  N/A
*/
#if 0
static void videoRecStampAligSet()
{
	UINT8 aligFmt;
	UINT16 offX, offY, offXStamp, offYStamp;

	aligFmt = videoRecStampAligFmtGet();

	offX = 16;	//default left and top
	offY = 16;
	offXStamp = 0;	//related to stamp
	offYStamp = 0;
    offXStamp = g_vidDateStamp.stampW - g_vidDateStamp.dateW;
    offYStamp = g_vidDateStamp.stampH - g_vidDateStamp.ftH;
 #if 0
	if(aligFmt & (UINT8)(VD_STAMP_RIGHT)){
		offX = g_vidDateStamp.imgW - g_vidDateStamp.stampW - 16;
		offXStamp = g_vidDateStamp.stampW - g_vidDateStamp.dateW;
	}
	else if(aligFmt & (UINT8)(VD_STAMP_CENTER)){
		offX = (g_vidDateStamp.imgW - g_vidDateStamp.stampW) >> 1;
		offXStamp = (g_vidDateStamp.stampW - g_vidDateStamp.dateW) >> 1;
	}

	if(aligFmt & (UINT8)(VD_STAMP_BOTTOM)){
		offY = g_vidDateStamp.imgH - g_vidDateStamp.stampH - 16;
		offYStamp = g_vidDateStamp.stampH - g_vidDateStamp.ftH;
	}
	else if(aligFmt & (UINT8)(VD_STAMP_VCENTER)){
		offY = (g_vidDateStamp.imgH - g_vidDateStamp.stampH) >> 1;
		offYStamp = (g_vidDateStamp.stampH - g_vidDateStamp.ftH) >> 1;
	}
 #endif
    //wangrf++ 1.13
      if(g_vidDateStamp.imgW - g_vidDateStamp.stampW - 16>g_vidParam[VD_STAMP_DISOFFX])
      {
        if(g_vidParam[VD_STAMP_DISOFFX]<16)
        {
           offX=16;
        }
		else
		{
      	   offX=g_vidParam[VD_STAMP_DISOFFX];
		}
		   offX &= 0xfff0;
      }
	  else
	  {
	  	offX=g_vidDateStamp.imgW - g_vidDateStamp.stampW-16;
		offX &= 0xfff0;
	  }
	  if(g_vidDateStamp.imgH -g_vidDateStamp.stampH-16>g_vidParam[VD_STAMP_DISOFFY] )
	  {

	  	if(g_vidParam[VD_STAMP_DISOFFY]<16)
        {
           offY=16;
        }
		else
		{
      	   offY=g_vidParam[VD_STAMP_DISOFFY];
		}
		   offY &= 0xfff0;
	  }
	  else
	  {
	  	offY = g_vidDateStamp.imgH - g_vidDateStamp.stampH - 16;
		offY &= 0xfff0;
	  }

    //wangrf++ end
	offXStamp &= ~1;

	g_vidDateStamp.disHOff = offX;
	g_vidDateStamp.disVOff = offY;
	if(g_vidParam[VD_STAMP_DATETIME_DIS_X_RATIO] < 64){//xian ++ 20090211
		g_vidDateStamp.dateHOff = g_vidDateStamp.stampW * g_vidParam[VD_STAMP_DATETIME_DIS_X_RATIO] /64;
		g_vidDateStamp.dateHOff  &= ~1;
		if(g_vidDateStamp.dateHOff + g_vidDateStamp.dateW >= g_vidDateStamp.stampW){
			g_vidDateStamp.dateHOff = offXStamp;
		}
	}
	else{
		g_vidDateStamp.dateHOff = offXStamp;
	}

	g_vidDateStamp.dateVOff = offYStamp;
	//DebugStampShow();
}
#endif
/**
 * @fn		  UINT8 videoRecDateStampInit(UINT16 imgW, UINT16 imgH)
 * @brief	  video Record DateStamp Initial
 * @param	  [in] imgW
 * @param	  [in] imgH
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-10
 * @todo	  N/A
 * @bug 	  N/A
*/
#if 0
static UINT8 videoRecDateStampInit(UINT16 imgW, UINT16 imgH)
{
	UINT16 ftW, ftH, dateW;
	UINT32 tmpAddr;
	UINT16 dateMode;
    //printf("\n img: %d\n",imgW);
	if(imgW < g_vidDateStamp.stampW || imgH < g_vidDateStamp.stampH){
		ASSERT(0, 1);
		return FAIL;
	}

	#if 0 //xian -- 20110624
   	//wangrf edit
	if(imgW<=320){
		ftW = DATE_STAMP_H_SIZE_6;
		ftH = DATE_STAMP_V_SIZE_6;
	}
	else if(imgW>720){ //(imgW>848){//modify for D1 size
		ftW = DATE_STAMP_H_SIZE_2;
		ftH = DATE_STAMP_V_SIZE_2;
		}
	else{
		ftW = DATE_STAMP_H_SIZE_3;
		ftH = DATE_STAMP_V_SIZE_3;
	}
   	//wangrf edit
   	#endif

	dateMode = (UINT16)(g_vidParam[VD_STAMP_DATATIME_FMT] & 0xffff);

	//xian ++ 20110624
	ftH = g_vidDateStamp.stampH;
	if((( dateMode & 0x100 ) == 0x100) &&  ( dateMode & 0x30 )){//two row
		ftH /= 2;
	}
	if(ftH >= DATE_STAMP_V_SIZE_2){
		ftH = DATE_STAMP_V_SIZE_2;
		ftW = DATE_STAMP_H_SIZE_2;
	}else if(ftH >= DATE_STAMP_V_SIZE_3){
		ftH = DATE_STAMP_V_SIZE_3;
		ftW = DATE_STAMP_H_SIZE_3;
	}else {
		ftH = DATE_STAMP_V_SIZE_6;
		ftW = DATE_STAMP_H_SIZE_6;
	}
	//xian --

	if((( dateMode & 0x100 ) == 0x100) &&  ( dateMode & 0x30 ))//two row
   	{
		dateW = ftW * (VID_STAMP_FNT_NUM - 10 );
   	}
	else
   	{
		if(dateMode & 0xf0)//data @ no time
		{
			dateW = ftW * (VID_STAMP_FNT_NUM);
		}
		else
		{
			dateW = ftW * (VID_STAMP_FNT_NUM - 9 );
		}
   	}

	if(dateW > g_vidDateStamp.stampW || ftH > g_vidDateStamp.stampH){
		ASSERT(0, 1);
		return FAIL;
	}

	g_vidDateStamp.imgW = imgW;
	g_vidDateStamp.imgH = imgH;

	g_vidDateStamp.ftW = ftW;
	g_vidDateStamp.ftH = ftH;
	g_vidDateStamp.dateW = dateW;

	tmpAddr = g_vidDateStamp.stampAddr;
	dateStampPrepare(g_vidDateStamp.ftAddr, tmpAddr, ftW, ftH); //load all font

	HAL_GprmDramDmaAdv(g_vidDateStamp.imgAddr<<1, g_vidDateStamp.stampAddr<<1, (UINT32)g_vidDateStamp.stampW*(g_vidDateStamp.stampH<<1), 1);
	g_vidDateStamp.disBufId = 0;

	videoRecStampAligSet();
	return SUCCESS;
}
#endif

//create stamp to stamAddr
/**
 * @fn		  void videoRecDateStampActive(UINT32 fntAddrWord, UINT16 fntW, UINT16 fntH, UINT32 stampAddrWord, UINT16 stampW, UINT16 stampH, UINT16 dateXOff, UINT16 dateYOff, UINT8 mode)
 * @brief	  videoRecDateStampActive
 * @param	  [in] fntAddrWord : font word address
 * @param	  [in] fntW : font width
 * @param	  [in] fntH : font height
 * @param	  [in] stampAddrWord : stamp word address
 * @param	  [in] stampW : stamp width
 * @param	  [in] stampH :  stamp height
 * @param	  [in] dateXOff : date x offset
 * @param	  [in] dateYOff : date y offset
 * @param	  [in] mode : time format . e.g. year/month/date
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
/*static void
videoRecDateStampActive(
	UINT32 fntAddrWord,
	UINT16 fntW,
	UINT16 fntH,
	UINT32 stampAddrWord,
	UINT16 stampW,
	UINT16 stampH,
	UINT16 dateXOff,
	UINT16 dateYOff,
	UINT16 mode
)
{
	dateStc_t videoDateInf;

	HAL_GlobalReadRTC(&videoDateInf);
	//dateStampActive( videoDateInf, fntAddrWord, stampAddrWord, stampW, stampH, dateXOff, dateYOff,fntW, fntH, 0, fntH, mode);
	dateStampActive( videoDateInf, fntAddrWord, fntW, fntH, stampAddrWord, stampW, stampH, dateXOff, dateYOff, mode);
}*/
/**
 * @fn		  void videoRecDateStampLoad(void)
 * @brief	  load date stamp
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
#if 0
static void videoRecDateStampLoad(UINT8 idx)
{
	UINT32 loadAddr;
	//UINT8 status;
	UINT16 mode;
	mode = (UINT16)(g_vidParam[VD_STAMP_DATATIME_FMT] & 0xffff);
	// videoDateInf;

	loadAddr=g_vidDateStamp.stampAddr+(vidRecMemSizeGet(VID_MEM_IDX_VIDEO_REC_StampA)/*K_SDRAM_AviStampBufSize*/*idx);

	HAL_GprmDramDmaAdv(g_vidDateStamp.imgAddr<<1, loadAddr<<1, (UINT32)g_vidDateStamp.stampW*(g_vidDateStamp.stampH<<1), 1);
	dateStampActive(videoDateInf, g_vidDateStamp.ftAddr, g_vidDateStamp.ftW, g_vidDateStamp.ftH,
		loadAddr, g_vidDateStamp.stampW, g_vidDateStamp.stampH,
		g_vidDateStamp.dateHOff, g_vidDateStamp.dateVOff,
		mode );
	videoTimeNext(&videoDateInf);
	videoDtFlag[idx]=1;
}

#endif
#if 0
void videoRecDateStampLoadInit()
{
	UINT8 i;
	UINT16 mode;
	UINT32 loadAddr;
	extern SINT32 audDataSize;
	//videoDtFlag[g_vidDateStamp.disBufId]=0;
	mode = (UINT16)(g_vidParam[VD_STAMP_DATATIME_FMT] & 0xffff);
	HAL_GlobalReadRTC(&videoDateInf);
	for(i=0;i<4;i++)
	{
		loadAddr=g_vidDateStamp.stampAddr+(vidRecMemSizeGet(VID_MEM_IDX_VIDEO_REC_StampA)/*K_SDRAM_AviStampBufSize*/*i);
		HAL_GprmDramDmaAdv(g_vidDateStamp.imgAddr<<1, loadAddr<<1, (UINT32)g_vidDateStamp.stampW*(g_vidDateStamp.stampH<<1), 1);
		dateStampActive(videoDateInf, g_vidDateStamp.ftAddr, g_vidDateStamp.ftW, g_vidDateStamp.ftH,
			loadAddr, g_vidDateStamp.stampW, g_vidDateStamp.stampH,
			g_vidDateStamp.dateHOff, g_vidDateStamp.dateVOff,
			mode );
		videoTimeNext(&videoDateInf);
		videoDtFlag[i]=1;
	}
	//videoDtFlag[0]=0;
	audDataSize=0;

}
#endif
#if 0
/**
 * @fn		  UINT8 videoRecImgStampInit(UINT16 imgW, UINT16 imgH)
 * @brief	  record stamp image initial
 * @param	  [in] imgW : date stamp width
 * @param	  [in] imgH :  date stamp height
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
static UINT8 videoRecImgStampInit(UINT16 imgW, UINT16 imgH)
{
	if(g_vidDateStamp.stampW > imgW || g_vidDateStamp.stampH > imgH){
		ASSERT(0, 1);
		return FAIL;
	}

	g_vidDateStamp.imgW = imgW;
	g_vidDateStamp.imgH = imgH;

	g_vidDateStamp.ftW = g_vidDateStamp.stampW;
	g_vidDateStamp.ftH = g_vidDateStamp.stampH;
	g_vidDateStamp.dateW = g_vidDateStamp.stampW;

	g_vidDateStamp.stampAddr = g_vidDateStamp.imgAddr; //only one buffer
	g_vidDateStamp.disBufId = 0;
	videoRecStampAligSet();

	return SUCCESS;
}
#endif
#if 0
/**
 * @fn		  UINT8 videoRecStampEofGet(void)
 * @brief	  get stamp eof flag
 * @param	  NONE
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 videoRecStampEofGet(void)
{
	return vidDateStampEof;
}

/**
 * @fn		  void videoRecStampEofSet(UINT8 eofFlag)
 * @brief	  set stamp eof flag
 * @param	  [in] eofFlag
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
void videoRecStampEofSet(UINT8 eofFlag)
{
	UINT32 sts;

	ENTER_CRITICAL(sts);
	vidDateStampEof = eofFlag;
	EXIT_CRITICAL(sts);
}

/**
 * @fn		  void INTR_videoRecStampEofSet(UINT8 eofFlag)
 * @brief	  set stamp eof flag in interrupt
 * @param	  [in] eofFlag
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-11
 * @todo	  N/A
 * @bug 	  N/A
*/
void INTR_videoRecStampEofSet(UINT8 eofFlag)
{
	vidDateStampEof = eofFlag;
}
#endif
#if 0
void videoTimeNext(pDateStc_t dateInfNext)
{
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

	//printf("*%bu\n",dateInfNext->Second);
}
#endif
UINT32 videoRecStampAddrGet(void)
{
    UINT32 curAddr;

    curAddr = g_vidDateStamp.stampAddr;
    if(g_vidDateStamp.disBufId){
        curAddr += K_SDRAM_AviStampBufSize;
    }
    return curAddr;
}
//xian --

//xian ++ 20090717
UINT32 INTR_videoRecStampAddrGet(void)
{
    UINT32 curAddr;

    curAddr = g_vidDateStamp.stampAddr;
    if(g_vidDateStamp.disBufId){
        curAddr += K_SDRAM_AviStampBufSize;
    }
    return curAddr;
}
//xian --

UINT32 videoRecStampSizeGet()
{
   return (UINT32)K_SDRAM_AviStampBufTotalSize;
}

UINT8 videoRecStampSet(UINT8 pasteThr,UINT16 stampW, UINT16 stampH, UINT16 fontW,UINT16 fontH)
{
	UINT8 ret = TRUE;
//	UINT8 frameRate;
	UINT16 pvW, pvH;
	UINT32 addr = vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_Text);//K_SDRAM_AviCaptureIdx1Addr + (UINT32)32*1024;
	UINT32 size;// = (UINT32)g_vidDateStamp.stampW*g_vidDateStamp.ftH*2;
	//printf("v1\n");
	videoRecSizeGet(&pvW, &pvH);
	if(pvW < g_vidDateStamp.stampW || pvH < g_vidDateStamp.stampH){
		ASSERT(0, 1);
		return FALSE;
	}

	if((UINT32)stampW * stampH > K_SDRAM_AviStampBufSize){
		ASSERT(0, 1);
		return FALSE;
	}

	g_vidDateStamp.imgW = pvW;
	g_vidDateStamp.imgH = pvH;
	g_vidDateStamp.stampW = stampW;
	g_vidDateStamp.stampH = stampH;
	g_vidDateStamp.ftW = fontW;
	g_vidDateStamp.ftH = fontH;
	g_vidDateStamp.pasteThd = pasteThr;
#if 1
#if 0
	//printf("v2\n");
	aeStatusGet(SP1K_AE_FRAMERATE,&frameRate);

	if ((frameRate==SP1K_FRAME_RATE_60)||(frameRate==SP1K_FRAME_RATE_50)){
		g_vidDateStamp.stampAddr = (UINT32)K_SDRAM_AviPvwBufAddrD_60FPS - (UINT32)K_SDRAM_AviStampBufTotalSize;	//A, B buffer
	}
	else{
		g_vidDateStamp.stampAddr = (UINT32)K_SDRAM_AviPvwBufAddrD - (UINT32)K_SDRAM_AviStampBufTotalSize;	//A, B buffer
	}

	g_vidDateStamp.ftAddr = g_vidDateStamp.stampAddr  + (K_SDRAM_AviStampBufSize * 4/*<<1*/); //fnt buffer
	g_vidDateStamp.imgAddr = g_vidDateStamp.ftAddr + K_SDRAM_AviStampFontBufSize; //img buffer
#else
	if(g_vidParam[VD_CAPTURE]){
		g_vidDateStamp.stampAddr = speedyCaptureMemAddrGet(VID_MEM_IDX_VIDEO_REC_StampA);
		g_vidDateStamp.ftAddr = speedyCaptureMemAddrGet(VID_MEM_IDX_VIDEO_REC_Font);
		g_vidDateStamp.imgAddr = speedyCaptureMemAddrGet(VID_MEM_IDX_VIDEO_REC_IMG);
	}
	else{
		g_vidDateStamp.stampAddr = vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_StampA);
		g_vidDateStamp.ftAddr = vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_Font);
		g_vidDateStamp.imgAddr = vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_IMG);
	}

#endif
	/*//read res image
	if(resId){
		sp1kNandRsvRead((UINT32)resFileId, (g_vidDateStamp.imgAddr<< 1));
	}else{
		gprmBlackFill(g_vidDateStamp.imgAddr, stampW, stampH);
	}*/

	//set font buf and w,h
	stampFontSet(g_vidDateStamp.ftAddr,g_vidDateStamp.ftW,g_vidDateStamp.ftH,g_vidDateStamp.stampAddr);
	//set background buf and w,h
	stampBackgroundSet(g_vidDateStamp.imgAddr,g_vidDateStamp.stampW,g_vidDateStamp.stampH);
	// enable copy background image when combine
	stampBackgroundEnable(1);

	size = g_vidDateStamp.stampW*g_vidDateStamp.ftH*2;
	// set date temp buf, speed up when updating
	//stampTextImageSet(STAMP_DATE, addr, size);
	// set time temp buf, speed up when updating
	//stampTextImageSet(STAMP_TIME, addr+size/2, size);
	// set time temp buf, speed up when updating
	//stampTextImageSet(STAMP_TEXT, addr+size/2, size);
#endif
	return TRUE;
}

//-----------------------------------------------------------------------------
//videostamp
//-----------------------------------------------------------------------------
/**
 * @fn		  void videoRecEofIntrEnable(void)
 * @brief	  Enable video compression completed interrupt
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	 rf.wang
 * @since	  2010-12-16
 * @todo	  N/A
 * @bug 	  N/A
*/
void videoStampSet()
{
	UINT32 srcAddr;
	UINT16 ysize;
	UINT16 disHOff,disVOff;
	//UINT16 stampW;
	//ysize &= 0x01FF;  // only [8:0] is valid
	if(g_vidParam[VD_REC_YUV_FORMAT]==VD_REC_YUV_422)
	{
		ysize=(g_vidDateStamp.stampH>>3)& 0x01FF;
		disHOff=g_vidDateStamp.disHOff>>4;
		disVOff=g_vidDateStamp.disVOff>>3;
	}
	if(g_vidParam[VD_REC_YUV_FORMAT]==VD_REC_YUV_420)
	{
		ysize=(g_vidDateStamp.stampH>>4)& 0x01FF;
		disHOff=g_vidDateStamp.disHOff>>4;
		disVOff=g_vidDateStamp.disVOff>>4;
	}
	if(g_vidParam[VD_REC_YUV_FORMAT]==VD_REC_YUV_422_420)
	{
		ysize=(g_vidDateStamp.stampH>>3)& 0x01FF;
		disHOff=g_vidDateStamp.disHOff>>4;
		disVOff=g_vidDateStamp.disVOff>>3;
	}

	srcAddr = videoRecStampAddrGet();
	// stampW=g_vidDateStamp.stampW>>1;
	XBYTE[REG_Dram_StampHSize_Low] = *((UINT8*)&g_vidDateStamp.stampW+1);
	XBYTE[REG_Dram_StampHSize_High] = *((UINT8*)&g_vidDateStamp.stampW+0);
	XBYTE[REG_Dram_StampStAddr_B0] = *((UINT8*)&srcAddr+3); //stamp addr
	XBYTE[REG_Dram_StampStAddr_B1] = *((UINT8*)&srcAddr+2);
	XBYTE[REG_Dram_StampStAddr_B2] = *((UINT8*)&srcAddr+1);
	XBYTE[REG_Dram_StampStAddr_B3] = *((UINT8*)&srcAddr+0);
	XBYTE[REG_Jpeg_Stamhoffset_Low] = *((UINT8*)&disHOff+1); //stamp h offset
	XBYTE[REG_Jpeg_Stamhoffset_High] = *((UINT8*)&disHOff+0);
	XBYTE[REG_Jpeg_Stamvoffset_Low] = *((UINT8*)&disVOff+1);//stamp v offset
	XBYTE[REG_Jpeg_Stamvoffset_High] = *((UINT8*)&disVOff+0);
	XBYTE[REG_Jpeg_Stmhsize] = g_vidDateStamp.stampW >>4;//g_vidDateStamp.stampW; //stamp h size

	XBYTE[REG_Jpeg_Stamvsize_Low] = *((UINT8*)&ysize+1);//stamp v size
	XBYTE[REG_Jpeg_Stamvsize_High] = *((UINT8*)&ysize+0);

	XBYTE[REG_Jpeg_Stampthr] = g_vidDateStamp.pasteThd;//  Y thr
	XBYTE[REG_Jpeg_StampEn] = 0x01;

    //dramFileSave("XXXXXX01.YUV", srcAddr, (UINT32)160*32*2);
	//dramFileSave("wangrf01YUV", srcAddr, (UINT32)640*32);
	// dramFileSave("UUUUIIIIYUV", g_vidDateStamp.stampAddr,(UINT32)320*96*2);
	//dramFileSave("UUUUWWWWYUV", 61440+g_vidDateStamp.stampAddr,(UINT32)320*96*2);
}
/**
 * @fn        void pvvideostampIntr()
 * @brief     videostampSetIntr
 * @param     [in] NULL
 * @retval    NONE
 * @see       NULL
 * @author    rf.wang
 * @since     2010-12-16
 * @todo      N/A
 * @bug       N/A
*/
void videoStampSetIntr()
{
#if 0
	UINT32 srcAddr;
	UINT32 audSize;
	extern UINT32 ringBufSize;
	extern UINT8 vidOsdTexts[2][4][20];
	extern video_osd_text_t vidOsdText[2];
	*((UINT8*)&audSize ) = 0;
	*((UINT8*)&audSize+1) = XBYTE[REG_Audio_DataCnt2];
	*((UINT8*)&audSize+2) = XBYTE[REG_Audio_DataCnt1];
	*((UINT8*)&audSize+3) = XBYTE[REG_Audio_DataCnt0];

	audSize = (audSize<< 1) % ringBufSize;
	audSize += (vidClipCtrl.AudChunkCnt<<SECPERCHK);
	if(audDataSize<(SINT32)audSize){
		UINT8 idx = g_vidDateStamp.disBufId+1;
		if(idx>=4){idx=0;}
		if(videoDtFlag[idx]){
			srcAddr=g_vidDateStamp.stampAddr+K_SDRAM_AviStampBufSize*idx;
			XBYTE[REG_Dram_StampStAddr_B0] = *((UINT8*)&srcAddr+3); //stamp addr
			XBYTE[REG_Dram_StampStAddr_B1] = *((UINT8*)&srcAddr+2);
			XBYTE[REG_Dram_StampStAddr_B2] = *((UINT8*)&srcAddr+1);
			XBYTE[REG_Dram_StampStAddr_B3] = *((UINT8*)&srcAddr+0);
			audDataSize+=audBytesPerSec;//=audSize/bytePerSec*bytePerSec+bytePerSec;
			videoDtFlag[g_vidDateStamp.disBufId]=0;
			g_vidDateStamp.disBufId=idx;
			vidOsdText[STAMP_DATE].text= vidOsdTexts[STAMP_DATE][idx];
			vidOsdText[STAMP_TIME].text= vidOsdTexts[STAMP_TIME][idx];
		}
	}

#else
	UINT32 srcAddr;
	UINT8 idx;
	UINT8 cnt;
	if(vidRTCCount>vidRTCCount0){
		cnt = vidRTCCount-vidRTCCount0;
		if(vidRTCCount>100){vidRTCCount=0;}
		vidRTCCount0 = vidRTCCount;
		idx = g_vidDateStamp.disBufId+cnt;
		if(idx>=4){idx=0;}
		if(videoDtFlag[idx]){
			//INTR_putchar('E');
			srcAddr=g_vidDateStamp.stampAddr+K_SDRAM_AviStampBufSize*idx;
			XBYTE[REG_Dram_StampStAddr_B0] = *((UINT8*)&srcAddr+3); //stamp addr
			XBYTE[REG_Dram_StampStAddr_B1] = *((UINT8*)&srcAddr+2);
			XBYTE[REG_Dram_StampStAddr_B2] = *((UINT8*)&srcAddr+1);
			XBYTE[REG_Dram_StampStAddr_B3] = *((UINT8*)&srcAddr+0);
			audDataSize+=audBytesPerSec;//=audSize/bytePerSec*bytePerSec+bytePerSec;
			videoDtFlag[g_vidDateStamp.disBufId]=0;
			g_vidDateStamp.disBufId=idx;
			vidOsdText[STAMP_DATE].text= vidOsdTexts[STAMP_DATE][idx];
			vidOsdText[STAMP_TIME].text= vidOsdTexts[STAMP_TIME][idx];
		}
	}
#endif

}

void videoCaptureStampSetIntr()
{
	UINT32 srcAddr;
	UINT8 idx;

	idx = g_vidDateStamp.disBufId+1;
	if(idx>=4){
		idx=0;
	}
	if(videoDtFlag[idx]){
		srcAddr=g_vidDateStamp.stampAddr+K_SDRAM_AviStampBufSize*idx;
		XBYTE[REG_Dram_StampStAddr_B0] = *((UINT8*)&srcAddr+3); //stamp addr
		XBYTE[REG_Dram_StampStAddr_B1] = *((UINT8*)&srcAddr+2);
		XBYTE[REG_Dram_StampStAddr_B2] = *((UINT8*)&srcAddr+1);
		XBYTE[REG_Dram_StampStAddr_B3] = *((UINT8*)&srcAddr+0);
		videoDtFlag[g_vidDateStamp.disBufId]=0;
		g_vidDateStamp.disBufId=idx;
	}
}

//xian ++ 20081103
void videoRecStampEnable(UINT8 enable, UINT8 init)
{
	if (enable) {
		if (init){
			pvPhofrmInit();
		}
		blendingMode = PRV_BLEND_DATESTAMP;

		#if MANU_CDSP_IDX
	    	XBYTE[REG_Dram_MpgSb_RegIdx_En] |= 0x02; //xian ++ 20090717
		#endif
		#if !VIDREC_JPEG_EOF_EN
		XBYTE[REG_Dram_Cdsp1Eof_Inten] |= 0x02;
		#endif
	}
 	else {
		#if MANU_CDSP_IDX
	   		XBYTE[REG_Dram_MpgSb_RegIdx_En] &= ~0x02; //xian ++ 20090717
		#endif
		XBYTE[REG_Dram_Cdsp1Eof_Inten] &= 0xFD;
	}
}

void videoRecStampSyncRtc()
{
	stampTextModifyByRTC(NULL);
	memset(videoDtFlag, 0, 4);
}

void videoRecRtcIntr()
{
	HAL_GlobalWriteRTCDataIntr(0xc0,0);
	vidRTCCount++;
	if( g_vidParam[VD_SLOW_REC_MOD] ){
		if(!(vidRTCCount%vidClipCtrl.slowRecFps)){
			g_SlowSnapOpen =0;
		}
	}
	//INTR_putchar('r');
}
