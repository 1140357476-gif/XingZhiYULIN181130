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
 * @file		pb_single.c
 * @brief		single picture play back function
 * @author    Tq.Zhang
 * @since	2010-05-05
 * @date		2010-05-05
*/
/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "hal.h"
#include "initio.h"
#include "os_msg.h"
#include "hal_disp.h"
#include "func_ptr.h"
#include "sdramhw.h"
//#include "dosusr.h"
#include "doslink.h"
#include "impt.h"
#include "disp_api.h"
#include "cardlink.h"
//#include "exifhdr.h"
#include "exiffunc.h"
#include "fileindex.h"
#include "pb_api.h"
#include "pb_debug.h"
#include "pb_general.h"
#include "sp1k_snap_api.h"
#include "sp1k_pb_api.h"
#include "sp1k_osd_api.h"
#include "sp1k_util_api.h"
#include "app_osd_icon.h"
#include "app_ui_osd.h"
#include "dos32.h"
#include "hal_jpeg.h"
#include "hal_cdsp.h"
#include "hal_gprm.h"
#include "hal_dram.h"
#include "disp_api.h"
#include "vfs_api.h"
#include "dcf_api.h"
#include "os_api.h"
#include "app_msg.h"


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

#define _PB_FADEIN_		0
UINT8 SlideStopFlag; //add for #37067
#if (!STILL_LIB_PB_DMY)
#define PB_CHECK_ABORT()	if(osMsgSeek(SP1K_MSG_KEY_PRESS_LEFT) ||osMsgSeek(SP1K_MSG_KEY_PRESS_RIGHT)) {\
								break;\
							}
/*************************************************
* Note:
*	jintao.liu 2008-6-19 add slide show state
*	for mantis #26566
*/
#define SLIDE_PLAY		1
#define SLIDE_STOP		0


/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
PBFile_t xdata MediaInfo;

/*************************************************
* Note:
*	jintao.liu 2008-6-19 add slide show state
*	for mantis #26566
*/
static UINT8 slideState;//as above
UINT8 pbDZoomKey;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
//extern void gprmBlackFill(UINT32 dstAddr, UINT16 imgW, UINT16 imgH);
extern UINT8 pbVideoFrameDecode(UINT16 FileIndex, UINT16 decWidth, UINT16 decHeight, UINT32 freeBufAddr, UINT8 fillBlack);

extern UINT16 xdata G_USR_FileCount;
extern UINT8 xdata RelatedWavFileName[];
extern UINT8 xdata imgScaleFac;
extern UINT8 xdata pbDZoomSts;
extern UINT32 aviFileLength;

extern RECT16 g_pb_rec_RotOut;
#endif
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

//=============================================================================
//Program
//=============================================================================
//-----------------------------------------------------------------------------
//pbSingleImgPlay
//-----------------------------------------------------------------------------
/**
 * @fn		UINT8 pbSingleImgPlay(UINT16 FileIndex)
 * @brief		single image play
 * @param	fileindex: current file index
 * @retval	SUCCESS / FAIL
 * @see		NONE
 * @author	Phil Lin
 * @since	2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
#if STILL_LIB_PB_DMY
UINT8 pbSingleImgPlay(UINT16 FileIndex) USING_0
{
	//printf("dumy single play!\n");
	FileIndex=FileIndex;
	return SUCCESS;
}
#else
UINT8 pbSingleImgPlay(UINT16 fileIndex) USING_0
{
	UINT8 ret;
	PBFile_t *pFileInfo = &MediaInfo;
	//UINT32 dispAddr;
	UINT8 decOpt = PB_DECODE_OPT_QVGA | PB_DECODE_OPT_FULL;
	UINT8 prm=(UINT8)pbParamGet(PB_PARAM_SPLAY_MODE) & ~PB_SPLAY_MODE_ZOOM1X;
	UINT8 prm_1x=(UINT8)pbParamGet(PB_PARAM_SPLAY_MODE) & PB_SPLAY_MODE_ZOOM1X;
    UINT16 decWidth;  
	UINT16 decHeight;
	UINT8 dev =0;

//	dispImgWinEn(0);

	if(dispDevActGet() != 1){
		dev = 1;
	}
	pbLogPrintf("index is %u\n", fileIndex);
	if (pbFileInfoGet(fileIndex, pFileInfo) != SUCCESS) {
		return FAIL;
	}
	pFileInfo->FileIdx = fileIndex;
	sysMemStillPbDispAddrUpdate(dispDevActGet() ==4);

	//if (dispAddr != pbDispBuffGet(1)) {//disp last buf (before zoom?)
	if(pbDispBufModeGet() == PB_DISP_BUF_MODE_ZOOM){
		#if 0 //xian --, fix flash
		HAL_PbBufAddrSet(pbDispBuffGet(1));

		if (dispDevActGet() !=4){
			HAL_PbBufHsizeSet(PB_DECODE_WIDTH);
			HAL_DispScaleSet(PB_DECODE_WIDTH, PB_DECODE_HEIGHT, 100,0);
		}else{
			HAL_PbBufHsizeSet(960);
			HAL_DispScaleSet(960, 720, 100,0);
		}
		HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PB);//update pb layer
		HAL_DramZUpdateWait(HAL_DRAM_ZUPDATE_PB, 0);
		#else
		#if 0 //xian -- 20110214
		if (dispDevActGet() !=4){
			//dispShow(pbDispBuffGet(1), PB_DECODE_WIDTH, PB_DECODE_HEIGHT, 100);
			dispShow(pbDispBuffGet(1), decWidth, decHeight, 100);
		}else{
			dispShow(pbDispBuffGet(1), 960, 720, 100);
		}
		#else		
		pbDispQVSizeGet(&decWidth,&decHeight); //last AB buff for QV
		dispShow(pbDispBuffGet(1), decWidth, decHeight, 100);
		#endif
		#endif
	}
	
	switch(prm)
	{
	case PB_SPLAY_MODE_THUMB:
		decOpt=PB_DECODE_OPT_THUMB | PB_DECODE_OPT_FULL;
		break;
	case PB_SPLAY_MODE_QV:
		decOpt=PB_DECODE_OPT_QVGA | PB_DECODE_OPT_FULL;
		break;
	case PB_SPLAY_MODE_MAIN:
		decOpt=PB_DECODE_OPT_FULL;
		break;
	default:
		break;
	}	
	
	ret = pbImgDecode(pFileInfo,
					K_SDRAM_ImagePlaybackBufAddr,
					decOpt
	);

	pbDZoomSts	= FALSE;

	#if 0 //xian -- 20110214
	/* if file error, clear disp frame buffer. */
	if ( ret != SUCCESS ) {
		if (dispDevActGet() !=4){
			gprmBlackFill(pbDispBuffGet(1), PB_DECODE_WIDTH, PB_DECODE_HEIGHT);
		}else{
			gprmBlackFill(pbDispBuffGet(1), 960, 720);
		}
	}
	else if(prm_1x==PB_SPLAY_MODE_ZOOM1X){
	#else
	pbDispBuffSwitch();
	if(ret == SUCCESS){
		pbDecodeSizeGet(&decWidth, &decHeight);
		HAL_GprmCopy(
			K_SDRAM_ImagePlaybackBufAddr,
			decWidth, decHeight, 0, 0,
			pbDispBuffGet(1),
			decWidth, decHeight, 0, 0,
			decWidth, decHeight,
			0,
			1
		);		
	}
	else{//file error
		if (dispDevActGet() !=4){
			decWidth = PB_DECODE_WIDTH;
			decHeight = PB_DECODE_HEIGHT;
		}else{
			decWidth = 960;
			decHeight = 720;
		}
		gprmBlackFill(pbDispBuffGet(1), decWidth, decHeight);
	}
	dispShow(pbDispBuffGet(1), decWidth, decHeight, 100); 
	pbDispQVSizeSet(decWidth, decHeight);	
	#endif
	
	if(ret == SUCCESS && prm_1x==PB_SPLAY_MODE_ZOOM1X){
		if(((osMsgSeek(SP1K_MSG_KEY_RELEASE_LEFT) == FALSE) &&(osMsgSeek(SP1K_MSG_KEY_REPEATE_LEFT) == TRUE))
			||((osMsgSeek(SP1K_MSG_KEY_RELEASE_RIGHT) == FALSE) &&(osMsgSeek(SP1K_MSG_KEY_REPEATE_RIGHT) == TRUE)))

		{
			pbDZoomKeySet(0);
		}
		else
		{
			pbDZoomKeySet(1);
			if(dev){
				ret=pbDZoomStart();
			}
		}

	}

//	dispImgWinEn(1);

	return ret;
}
#endif

#if (!STILL_LIB_PB_DMY)
void pbDZoomKeySet(UINT8 value)
{
	pbDZoomKey=value;

}
#endif

#if STILL_LIB_PB_DMY
UINT8 pbDZoomKeyGet(void)
{
	return 0;
}
#else
UINT8 pbDZoomKeyGet(void)
{
	return pbDZoomKey;
}
#endif
//-----------------------------------------------------------------------------
//pbVideoFramePlay
//-----------------------------------------------------------------------------
/**
 * @fn		UINT8 pbVideoFramePlay(UINT16 FileIndex)
 * @brief		play the first frame of video
 * @param	fileindex: current file index
 * @retval	SUCCESS / FAIL
 * @see		NONE
 * @author	Phil Lin
 * @since	2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
#if STILL_LIB_PB_DMY
UINT8 pbVideoFramePlay(UINT16 FileIndex)USING_0
{
        FileIndex=FileIndex;
        return SUCCESS;
}
#else
extern UINT16 decW_HDMI;
extern UINT16 decH_HDMI;
UINT8 pbVideoFramePlay(UINT16 FileIndex) USING_0
{
	UINT32 backAddr = K_SDRAM_ImagePlaybackBufAddr;
	UINT16 decWidth = PB_DECODE_WIDTH;
	UINT16 decHeight = PB_DECODE_HEIGHT;
	UINT8 sts ;
    //UINT32 dispAddr;
	UINT16 QVdsipW;  //CX++
	UINT16 QVdsipH;

	pbDZoomSts	= FALSE; //xian ++ , fix #41989

	sysMemStillPbDispAddrUpdate(dispDevActGet() ==4);
	osSysModeSet(OS_SYS_MODE_STILL_PB);
    //xian ++ 20090423, #34597
	//HAL_PbBufAddrGet(&dispAddr);
	#if 0 //xian --
	if (dispAddr != pbDispBuffGet(1)) {
		HAL_PbBufAddrSet(pbDispBuffGet(1));
		if (dispDevActGet() !=4){
			HAL_PbBufHsizeSet(PB_DECODE_WIDTH);
			HAL_DispScaleSet(PB_DECODE_WIDTH, PB_DECODE_HEIGHT, 100,0);
		}else{
			HAL_PbBufHsizeSet(960);
			HAL_DispScaleSet(960, 720, 100,0);
		}
		HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PB);//update pb layer
	}
	#else
	if(pbDispBufModeGet() == PB_DISP_BUF_MODE_ZOOM){
		//printf("2^^ %lx\n", dispAddr);
		#if 0 //xian -- 20110214
		if (dispDevActGet() !=4){
			//dispShow(pbDispBuffGet(1), PB_DECODE_WIDTH, PB_DECODE_HEIGHT, 100);
			dispShow(pbDispBuffGet(1), QVdsipW, QVdsipH, 100);
		}
		else{
			dispShow(pbDispBuffGet(1), 960, 720, 100);
		}		
		#else
		pbDispQVSizeGet(&QVdsipW,&QVdsipH); //last AB buff for QV
		dispShow(pbDispBuffGet(1), QVdsipW, QVdsipH, 100);
		#endif
	}
	#endif
	//xian --

	if (dispDevActGet() == 4 || dispDevActGet() == 2){
		decWidth = 960;
		decHeight = 720;
	}
    sts = pbVideoFrameDecode(FileIndex, decWidth, decHeight, backAddr, 0);    
	pbDispBuffSwitch();
    if(sts == SUCCESS){
		//xian -- 20110214
		//SY add for HD play on HDMI s	
		//if(decW_HDMI != decWidth || decH_HDMI != decHeight){
		//	decWidth = decW_HDMI;
		//	decHeight = decH_HDMI;
		//	decW_HDMI = PB_DECODE_WIDTH;
		//	decH_HDMI = PB_DECODE_HEIGHT;
		//}
		//SY add for HD play on HDMI e

		#if 0 //xian -- 20110214
		decWidth = g_pb_rec_RotOut.hSize;
		decHeight = g_pb_rec_RotOut.vSize;

    	pbDecodeSizeSet(decWidth, decHeight);
		#else
    	pbDecodeSizeGet(&decWidth, &decHeight);
		#endif
    	HAL_GprmCopy(
    		backAddr,
    		decWidth, decHeight, 0, 0,
    		pbDispBuffGet(1),
    		decWidth, decHeight, 0, 0,
    		decWidth, decHeight,
    		0,
    		1
    	);
    }
    else{
		//xian -- 20110214
		//pbDecodeSizeGet(&decWidth, &decHeight);
		gprmBlackFill(pbDispBuffGet(1), decWidth, decHeight);
    }
	dispShow(pbDispBuffGet(1), decWidth, decHeight, 100);		
	pbDispQVSizeSet(decWidth, decHeight);	
	
    return sts;
    
}
#endif
//-----------------------------------------------------------------------------
//pbVoiceMemoCheck
//-----------------------------------------------------------------------------
/**
 * @fn		UINT8 pbVoiceMemoCheck(UINT16 FileIndex)
 * @brief		check if there is any voice memo releated to current jpg
 * @param	fileindex: file index
 * @retval	TRUE / FALSE
 * @see		NONE
 * @author	Phil Lin
 * @since	2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
#if STILL_LIB_PB_DMY
UINT8 pbVoiceMemoCheck(UINT16 FileIndex)
{
        FileIndex=FileIndex;
        return SUCCESS;
}
#else
UINT8 pbVoiceMemoCheck(UINT16 FileIndex)
{
	UINT16 fHandle;
	PBFile_t *pFileInfo = &MediaInfo;
	UINT8 AudioFileName[12] = {0};
	UINT8 ret = FALSE;

	if ((pbFileInfoGet(FileIndex, pFileInfo) != SUCCESS)) {
		DBG_pbFuncTrack();
		return FAIL;
	}

	memcpy(AudioFileName, pFileInfo->FileName, sizeof(AudioFileName));
	AudioFileName[8] = 'W';
	AudioFileName[9] = 'A';
	AudioFileName[10] = 'V';

	//printf("phil:%s\n", AudioFileName);

	fHandle = vfsFileOpen(AudioFileName, K_DOS_ReadFile, 0);
	if (fHandle) {
		ret = TRUE;
		//CPrintf("Voice memo!");
	}
	vfsFileClose(fHandle,  AudioFileName, K_DOS_ReadFile, 0);
	return ret;
}
#endif
//-----------------------------------------------------------------------------
//pbFilePlayFadeIn
//-----------------------------------------------------------------------------
/**
 * @fn		UINT8 pbFilePlayFadeIn(UINT16 Time)
 * @brief		pbFilePlayFadeIn
 * @param	Time: delay X ms
 * @retval	SUCCESS
 * @see		NONE
 * @author	Phil Lin
 * @since		2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
#if _PB_FADEIN_
#if STILL_LIB_PB_DMY
UINT8 pbFilePlayFadeIn(UINT16 Time)
{
        Time=Time;
        return SUCCESS;
}
#else
UINT8 pbFilePlayFadeIn(UINT16 Time)
{
	UINT8 i;
	UINT8 tmpBlendIdx;

	tmpBlendIdx = XBYTE[REG_Disp_OsdV1L];

	for (i = 1; i < 8; i++) {
		PB_CHECK_ABORT();
		osdLocalBlendSet(0, i);//TODO: should be modified for 1628
		HAL_DispSignalWait(HAL_DISP_WAIT_VSYNC, HAL_DISP_WAIT_FALLING);
		osdRegUpdate();
		osdClear(OSD_ATTR_COLOR_WHITE | OSD_ATTR_MODE_HALFTONE | OSD_ATTR_BACK_EN);
		PB_CHECK_ABORT();
		HAL_Wait(Time);
	}

	osdClear(0);
	XBYTE[REG_Disp_OsdV1L] = tmpBlendIdx;
	osdRegUpdate();

	return SUCCESS;
}
#endif
#endif


/**
* @fn		UINT8 pbSlideShowStateGet(void)
* @brief		pbSlideShowStateGet
* @param	NONE
* @retval	return = SUCCESS / FAIL
* @see		NULL
* @author	Tq.Zhang
* @since		2010-5-5
* @todo		N/A
* @bug		N/A
*/
#if STILL_LIB_PB_DMY
UINT8 pbSlideShowStateGet(void)
{
	return 0; //0/1
}
#else
UINT8 pbSlideShowStateGet(void)
{
	return slideState;
}
#endif
/**
 * @fn		void pbSlideShowStateSet(UINT8 state)
 * @brief		pbSlideShowStateSet
 * @param     [in] state
 * @retval	NONE
 * @see		NULL
 * @author    Tq.Zhang
 * @since	2010-5-5
 * @todo		N/A
 * @bug		N/A
*/
#if (!STILL_LIB_PB_DMY)
void pbSlideShowStateSet(UINT8 state)
{
	slideState = state;
}
#endif
//-----------------------------------------------------------------------------
//pbSlideShowStart
//-----------------------------------------------------------------------------
/**
 * @fn		UINT8 pbSlideShowStart(void)
 * @brief		pbSlideShowStart
 * @param	none
 * @retval	SUCCESS
 * @see		NONE
 * @author	Phil Lin
 * @since	2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
/*************************
* Note:
*
*	make the function run
* every 1s.
*
*/
#if STILL_LIB_PB_DMY
UINT8 pbSlideShowStart(void)
{
        return SUCCESS;
}
#else
UINT8 pbSlideShowStart(void)
{
	TIMER1_Config(pbParamGet(PB_PARAM_SLIDE_SHOW_FREQ), pFunc_SlideShowProc, NULL);
	TIMER1_Start();
	pbSlideShowStateSet(SLIDE_PLAY);//jintao.liu 2008-6-19
	return SUCCESS;
}
#endif
//-----------------------------------------------------------------------------
//pbSlideShowAbort
//-----------------------------------------------------------------------------
/**
 * @fn		UINT8 pbSlideShowAbort(void)
 * @brief		pbSlideShowAbort
 * @param	none
 * @retval	SUCCESS
 * @see		NONE
 * @author	Phil Lin
 * @since	2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
#if STILL_LIB_PB_DMY
UINT8 pbSlideShowAbort(void)
{
	return SUCCESS;
}
#else
UINT8 pbSlideShowAbort(void)
{
	TIMER1_Stop();
	pbSlideShowStateSet(SLIDE_STOP);
	return SUCCESS;
}
#endif
/**
 * @fn		UINT8 pbSlideShowContinue(void)
 * @brief		pbSlideShowContinue
 * @param	NONE
 * @retval	return = SUCCESS / FAIL
 * @see		NULL
 * @author    Tq.Zhang
 * @since	2010-5-5
 * @todo		N/A
 * @bug		N/A
*/
#if STILL_LIB_PB_DMY
UINT8 pbSlideShowContinue(void)
{
	return SUCCESS;
}
#else
UINT8 pbSlideShowContinue(void)
{
TIMER1_Config(pbParamGet(PB_PARAM_SLIDE_SHOW_FREQ), pFunc_SlideShowProc, NULL);
	TIMER1_Start();
	pbSlideShowStateSet(SLIDE_PLAY);
	return SUCCESS;
}
#endif
//-----------------------------------------------------------------------------
//pbSlideShowProc
//-----------------------------------------------------------------------------
/**
 * @fn		UINT8 pbSlideShowProc(UINT16* FileIndex)
 * @brief		pbSlideShowProc
 * @param	none
 * @retval	SUCCESS
 * @see		NONE
 * @author	Phil Lin
 * @since	2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
#if STILL_LIB_PB_DMY
UINT8 pbSlideShowProc(UINT16* FileIndex)
{
        FileIndex=FileIndex;
        return SUCCESS;
}
#else
UINT8 pbSlideShowProc(UINT16* FileIndex)
{
	UINT8 ret = FAIL;
	PBFile_t fileInfo;
	UINT16 fileIdx;
	UINT16 lastFileIdx;
	UINT16 fileCnt;
	UINT16 i;
	UINT8 rpt;
	static UINT16 pbSlideCnt = 0;
	//add for mantis #37067@start
	if(SlideStopFlag==1)
	{
		pbSlideCnt=0;
		SlideStopFlag=0;
	}
	//add for mantis #37067@end
	fileCnt = dcfFileCountGet();
	if (!fileCnt || !FileIndex || *FileIndex > fileCnt) {
		return FAIL;
	}

	//switch to next file
	lastFileIdx = *FileIndex;
	*FileIndex = *FileIndex + 1;
	if (*FileIndex > fileCnt) {
		*FileIndex = 1;
	}
	//printf("fileIdx %u\n", fileIdx);
	fileIdx = *FileIndex;

	//Try to find jpg file
	for (i = 0; i < fileCnt; i++) {
		//printf("*** i %u\n", i);
		ret = pbFileInfoGet(DOS_FileIndexGet(fileIdx), &fileInfo);
		pbSlideCnt++;
		if (ret == SUCCESS && (fileInfo.FileType == SP1K_PB_FILE_TYPE_JPG)) {
			break;
		} else {
			if (++fileIdx > fileCnt) {
				fileIdx = 1;
			}
			*FileIndex = fileIdx;
			ret = FAIL;
		}
	}

	if (ret != SUCCESS) {
		//No more jpg, abort the slide show process
		//CPrintf("No more jpg");
		pbSlideShowAbort();
		return ret;
	}

	if (rpt == TRUE || pbSlideCnt <= fileCnt) {
#if (DBG_Level > 0) 		
		printf("Play -> %s\n", fileInfo.FileName);
#endif //(DBG_Level > 0)
		uiUpdateOSDPbFileNum();
		ret = pbSingleImgPlay(fileInfo.FileIdx);
#if _PB_FADEIN_
		pbFilePlayFadeIn(20);
#endif
	}

	rpt = pbParamGet(PB_PARAM_SLIDE_SHOW_RPT);
#if (DBG_Level > 1) 
	printf("cnt -> %u\n", pbSlideCnt);
#endif //(DBG_Level > 1)
//add for mantis #37044@start
	if(pbSlideCnt == fileCnt)
	{
		if (rpt != TRUE)
		{
			pbSlideShowAbort();
			osMsgPost(SP1K_MSG_SLIDE_SHOW_NEXT);
		}
		return ret;
		
	}
//add for mantis #37044@end

	if (pbSlideCnt > fileCnt) {
		pbSlideCnt = 0;
		//CPrintf("PlayEnd");
		if (rpt != TRUE) {
			pbSlideShowAbort();
			//reset fileindex
			*FileIndex = lastFileIdx;
			osMsgPost(SP1K_MSG_SLIDE_SHOW_FINISH);
			//return SUCCESS;
		}
	}

	return ret;
}
#endif
