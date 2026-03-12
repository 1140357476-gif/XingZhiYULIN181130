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
 * @file		pb_multi.c
 * @brief		multiply play back function
 * @author	tq.zhang
 * @since	2010-05-05
 * @date		2010-05-05
*/
/**************************************************************************
 *						  H E A D E R	F I L E S
 **************************************************************************/
#include "general.h"
#include "initio.h"
#include "sdramhw.h"
#include "disp_api.h"
//#include "dosusr.h"
#include "fileindex.h"
#include "pb_api.h"
#include "pb_general.h"
#include "doslink.h"
//#include "exifhdr.h"
#include "exiffunc.h"
#include "doslink.h"
#include "cardlink.h"
#include "snap_qtable.h"
#include "sp1k_snap_api.h"
#include "sp1k_pb_api.h"
#include "pb_debug.h"
#include "hal_jpeg.h"
#include "osd_api.h"
#include "app_ui_osd.h"
#include "smcfunc.h"
#include "sp1k_rsvblk_api.h"
#include "hal_gprm.h"
#include "hal.h"
#include "hal_dram.h"
#include "hal_global.h"
#include "vfs_api.h"
#include "dcf_api.h"
#include "gprm.h"
#include "osd_buff.h"
#include "os_api.h"

#include "exif_def.h"
#include "exif_entry.h"

/**************************************************************************
 *							 C O N S T A N T S							  *
 **************************************************************************/
#if (!STILL_LIB_PB_DMY)
 typedef struct{
	UINT8 resId;
	UINT8 aligFmt;
	UINT16 w;
	UINT16 h;
	UINT32 addr;
}pbIconInfo_t;

/**************************************************************************
 *								M A C R O S 							  *
 **************************************************************************/
 #define HDMI_BUF_W		((UINT16)960)
 #define HDMI_BUF_H		((UINT16)720)
/**************************************************************************
 *							D A T A    T Y P E S						  *
 **************************************************************************/
 
/**************************************************************************
 *						   G L O B A L	  D A T A						  *
 **************************************************************************/
static PBMultiImg_t xdata pbMpObj[PB_MULTIPLAY_LAYOUT_MAX];
static UINT8 xdata pbMpActiveObjIdx;
static PBMultiLayout_t xdata pbMpLayout;
static multiPbSelWin_t multiPbPos[PB_MULTIPLAY_LAYOUT_MAX];
static UINT8 multiIndex;
UINT8 multiPlayEn;
/**************************************************************************
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/
//extern void gprmBlackFill(UINT32 dstAddr, UINT16 imgW, UINT16 imgH);
extern UINT32 DOS_CurrDirGet(UINT8 DevId);
extern UINT8 DOS_CurrDriverGet(void);
extern UINT8 pbVideoFrameDecode(UINT16 FileIndex, UINT16 decWidth, UINT16 decHeight, UINT32 freeBufAddr, UINT8 fillBlack);
extern UINT8 dramFileSave(UINT8 *name,UINT32 startAddr,	UINT32 size);

extern xdata UINT8 exifVlcAtOdd;
extern UINT16 xdata pbParam[PB_PARAM_MAX];
extern PBFile_t xdata MediaInfo;
extern UINT32 aviFileLength;
//extern xdata UINT8 NeedReadMoreHeader;
//wangjg++
extern xdata UINT8 exifQvVlcAtOdd;
extern xdata UINT32 exifQvVlcAddr;
extern xdata UINT32 exifQvYTblAddr;
extern xdata UINT32 exifQvUVTblAddr;
extern xdata UINT8 exifAppN;
extern xdata UINT16 G_JpgRstMcuNum;
//wangjg--

/**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/
static UINT8 pbDrawIconType(UINT32 imgBuf, UINT16 imgW, UINT16 imgH, UINT8 iconParId, UINT32 tmpBuf);
static UINT8 pbMultiPlayVideoFrameDecode(PBMultiImg_t* pTumbLayout);
static UINT8 pbMultiPlayAudioBgDecode(PBMultiImg_t* pTumbLayout);
static UINT8 pbMultiPlayError(UINT32 resId, PBMultiImg_t* pTumbLayout);
UINT8 pbMultiPlayDecode(UINT32 pBufAddrWord, PBMultiImg_t* pTumbLayout, UINT8 DecOpt);
#endif
//=============================================================================
//Program
//=============================================================================
//-----------------------------------------------------------------------------
//pbMultiPlayLayoutInit
//-----------------------------------------------------------------------------
/**
 * @fn		  void pbMultiPlayLayoutInit(UINT8 LayoutType, UINT16 FileIndex)
 * @brief	  pbMultiPlayLayoutInit
 * @param	  [in] LayoutType : 2*2 / 3*3, default is 3*3
 * @param	  [in] FileIndex
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Phil Lin
 * @since	  2010-5-5
 * @todo	  N/A
 * @bug 	  N/A
*/
#if STILL_LIB_PB_DMY
UINT8 pbMultiPlayLayoutInit(UINT8 LayoutType, UINT16 FileIndex)
{
        LayoutType=LayoutType;
        FileIndex=FileIndex;
        return SUCCESS;
}
#else
UINT8 pbMultiPlayLayoutInit(UINT8 LayoutType, UINT16 FileIndex)
{
	UINT16 pbThumbW, pbThumbH;
	UINT16 pbThumbOffX, pbThumbOffY;
	UINT8  pbDispCol, pbDispRow;
	UINT8 i, j;
	UINT8 ret;
	UINT8 Idx;
	UINT16 fHandle;
	UINT8 fileName[12];
	PBMultiImg_t xdata* pMpImgObj;
	UINT16 decWidth; 
	UINT16 decHeight;
	devAttr_t* pnlDev;
	UINT16 thumbStartX, thumbStartY;
	UINT8 dev;

	dev = dispDevActGet();
	switch(dev){
		case 1:		//LCD
			 decWidth  = PB_DECODE_WIDTH;
	 		 decHeight = PB_DECODE_HEIGHT;
		break;
		case 2:		//NTSC
			 decWidth = 720;
	 		 decHeight = 480;
		break;
		case 3:		//PAL
			 decWidth = 720;
	 		 decHeight = 576;
		break;
		case 4:		//HDMI
			 decWidth = HDMI_BUF_W;
	 		 decHeight = HDMI_BUF_H;
		break; 
	}

	//memset(pMpImgObj, 0, sizeof(PBMultiImg_t));
	thumbStartX = pbParamGet(PB_PARAM_THUMB_START_X_Y) >> 8;
	thumbStartY = pbParamGet(PB_PARAM_THUMB_START_X_Y) & 0x00ff;

	if(pbParamGet(PB_PARAM_DISP_BUF_W)){        
		decWidth = pbParamGet(PB_PARAM_DISP_BUF_W);
		decHeight = pbParamGet(PB_PARAM_DISP_BUF_H);
	}
	decWidth -= (thumbStartX<<1);
	decHeight -= (thumbStartY<<1);
	//check layout type

	switch (LayoutType) {
		case PB_MULTIPLAY_LAYOUT_4:
			pbMpLayout.Type = PB_MULTIPLAY_LAYOUT_4;
			pbMpLayout.Max = 4;
			pbMpLayout.HNum = pbMpLayout.VNum = 2;
			pbDispCol = pbDispRow = 2;
			break;
		case PB_MULTIPLAY_LAYOUT_9:
			pbMpLayout.Type = PB_MULTIPLAY_LAYOUT_9;
			pbMpLayout.Max = 9;
			pbMpLayout.HNum = pbMpLayout.VNum = 3;
			pbDispCol = pbDispRow = 3;
			break;
		default:
			pbMpLayout.Type = PB_MULTIPLAY_LAYOUT_9;
			pbMpLayout.HNum = pbMpLayout.VNum = 3;
			pbMpLayout.Max = 9; //xian ++ 20080627
			pbDispCol = pbDispRow = 0;
			return FAIL;
			//break;
	}
    if(pbParamGet(PB_PARAM_THUMB_W_H) == 0){
    	pbThumbW = (((decWidth - ((pbDispCol + 1) << 3)) / pbDispCol) & ~0x0f);
    	pbThumbH = (((decHeight - ((pbDispRow + 1) << 3)) / pbDispRow) + 7) & ~0x07;
    }
    else{//xian ++ 20091126
        pbThumbW = (pbParamGet(PB_PARAM_THUMB_W_H) >> 8) & ~0x0f;
        pbThumbH = (pbParamGet(PB_PARAM_THUMB_W_H) & 0x00ff) & ~0x07;
    }
	pbThumbOffX = (((decWidth - pbDispCol * pbThumbW) / (pbDispCol + 1)) & ~0x07);
	pbThumbOffY = ((decHeight - pbDispRow * pbThumbH) / (pbDispRow + 1));

	pbMpLayout.Total = 0;
	pMpImgObj = &pbMpObj[0];

	if (FileIndex < pbMpLayout.Max) {
		pbMpActiveObjIdx = FileIndex;
		FileIndex = 1;
	} else {
		pbMpActiveObjIdx = (FileIndex % pbMpLayout.Max);
		if (pbMpActiveObjIdx) {
			FileIndex = (FileIndex / pbMpLayout.Max * pbMpLayout.Max) + 1;
		} else {
			pbMpActiveObjIdx = pbMpLayout.Max;
			FileIndex = ((FileIndex - 1) / pbMpLayout.Max * pbMpLayout.Max) + 1;
		}
	}
#if (DBG_Level > 1)
	printf("objIdx %bu, fileIdx %u\n", pbMpActiveObjIdx, FileIndex);//
#endif //(DBG_Level > 1)
/**/

	for (i = 0; i < pbDispRow; i++) {
		for (j = 0; j < pbDispCol; j++) {
			Idx = i * pbDispRow + j;

			//calculate layout offset

			pMpImgObj->OffsetX = pbThumbOffX + (pbThumbW + pbThumbOffX) * (UINT16)j;
			pMpImgObj->OffsetY = pbThumbOffY + (pbThumbH + pbThumbOffY) * (UINT16)i;
			pMpImgObj->ImgWidth = pbThumbW;
			pMpImgObj->ImgHeight = pbThumbH;

			pMpImgObj->OffsetX += thumbStartX;	
           		 pMpImgObj->OffsetY += thumbStartY;
			
			pnlDev = sp1kDevHdlGet();

			//lib pb version 1031, 20080821 jintao.liu
			/*if (0 == pnlDev[DEV_ID_PANEL].devEn)//TV plug
			{
				pMpImgObj->OffsetX -= 3;//move multi play thumb(s) to left a little
			}*/
			pMpImgObj->OffsetX &= ~1; //xian fix bug 31004

			//printf("index %u\n", FileIndex + (UINT16)Idx);
			ret = pbFileInfoGetExt(
				DOS_FileIndexGet(FileIndex + (UINT16)Idx),
				&pMpImgObj->pImgInfo,
				0
			);
			if (ret != SUCCESS) {
				//printf("idx %bu err\n", (FileIndex + (UINT16)Idx));
				pMpImgObj->VoiceMemo = 0;
				pMpImgObj->err = FAIL;
			} 
			else {
				//check voice memo
				strcpy(fileName, pMpImgObj->pImgInfo.FileName);
				if(pMpImgObj->pImgInfo.FileType == SP1K_PB_FILE_TYPE_JPG){
					fileName[8] = 'W';
					fileName[9] = 'A';
					fileName[10] = 'V';
					fHandle = vfsFileOpen(fileName, K_DOS_ReadFile, 0);
					if (fHandle) {
						pMpImgObj->VoiceMemo = 1;
					} else {
						pMpImgObj->VoiceMemo = 0;
					}
					vfsFileClose(fHandle, fileName, K_DOS_ReadFile, 0);
				}
				else{
					pMpImgObj->VoiceMemo = 0;
				}

				pbMpLayout.Total++;
				pMpImgObj->err = SUCCESS;
			}

			pMpImgObj++;
			//printf("idx = %bu, x = %u, y = %u\n", Idx, pbMpObj[Idx].OffsetX, pbMpObj[Idx].OffsetY);
		}
	}
	#if 0
	printf("dispW = %u, dispH = %u\n", decWidth, decHeight);
	printf("offX = %u, offY = %u\n", pbThumbOffX, pbThumbOffY);
	printf("W = %u, H = %u\n", pbThumbW, pbThumbH);
	#endif
	osdSelWinSet(-1, -1, -1, -1, 0, -1);
	osdRegUpdate();

}
#endif
//-----------------------------------------------------------------------------
//pbMultiPlayDecode
//-----------------------------------------------------------------------------
/**
 * @fn		UINT8 pbMultiPlayDecode(UINT32 pBufAddr, PBMultiImg_t* pTumbLayout, UINT8 DecOpt)
 * @brief		multiply picture play back decode
 * @param     [in] pBufAddrWord
 * @param     [in] pTumbLayout
 * @param     [in] DecOpt
 * @retval	return = SUCCESS / FAIL
 * @see		NULL
 * @author	Tq.Zhang
 * @since	2010-5-5
 * @todo		N/A
 * @bug		N/A
*/
#if (!STILL_LIB_PB_DMY)
UINT8 pbMultiPlayDecode(UINT32 pBufAddrWord, PBMultiImg_t* pTumbLayout, UINT8 DecOpt) USING_0
{
	UINT8 sts = TRUE;
//	UINT8 ret;
	UINT16 fHandle = TRUE;
	UINT32 decSize = 0;
//	UINT8 opt;
//	UINT16 srcWidth, srcHeight;
	UINT8 bShowThumb = 0;
	UINT8 mcuNum = 0;
	UINT8 imgScaleFac = 0;
	UINT32 filesize;
	UINT16 exifRot;
	ExifImage_t imagePara;
	ExifImage_t thumbPara;
	ExifImage_t* pPara;

	UINT16 decWidth;
	UINT16 decHeight;

	//xian ++ 20080618
	UINT8 rotMode;
	Pb_Dec_Parameter decPara;
	//xian --

	//xian ++ 20080703
	PBFile_t* pFile;
	pFile = &(pTumbLayout->pImgInfo);
	//xian --

	pbDispQVSizeGet(&decWidth, &decHeight);	//xian 20110214

	//4 To do a completely open process, start cluster must be 0
	fHandle =  vfsFileOpen(pFile->FileName, K_DOS_ReadFile, 0);
	if (!fHandle) {
		DBG_pbFuncTrack();
		return FAIL;
	}

	filesize = vfsFileSizeGet(fHandle);
	//Check the size of file parsing
	if (!(DecOpt & PB_DECODE_OPT_QVGA) && !(DecOpt & PB_DECODE_OPT_THUMB)) {
		pPara = &imagePara;
		decPara.ImgType = 0; //main image
	} else {
		pPara = &thumbPara;
		if (DecOpt & PB_DECODE_OPT_THUMB) {
			bShowThumb = 1;
		}
		decPara.ImgType = 1; //thumb image
	}

	//The size of exif header is always 64KB in 1528
	decSize = 64UL * 1024UL;
	if(decSize > filesize) //xian ++ 20081009
		decSize = filesize;
#if 	0
	printf("Name %s\n", pFile->FileName);
	printf("file size %lx\n", filesize);
#endif

	sts = vfsFileRead(fHandle, K_DOS_DstDram, decSize, pBufAddrWord, 0);

	if (sts != TRUE) {
		vfsFileClose(fHandle, pFile->FileName, K_DOS_ReadFile, 0);
		DBG_pbFuncTrack();
		return FAIL;
	}

	//NeedReadMoreHeader = 0; //xian ++ 20080911
	sts = exifParserStart(exifGetMainInfo,&imagePara, pBufAddrWord, decSize);
	sts = exifParserStart(exifGetThumbInfo,&thumbPara, pBufAddrWord, decSize);
	exifRot = imagePara.orientation;
	//sts = exifParsingFile(pBufAddrWord, decSize, bShowThumb, pFile->MediaTime, &ImagePara,&ThumbPara);
	//xian ++ 20080911	bug#31793
	if ((sts == FALSE) && (bShowThumb) || !thumbPara.width || !thumbPara.height || thumbPara.addr == imagePara.addr) {
		decSize = filesize;
		sts = vfsFileRead(fHandle, K_DOS_DstDram, decSize, pBufAddrWord, 0);
		if (sts) {
			sts = exifParserStart(exifGetMainInfo, &imagePara, pBufAddrWord, decSize);
			sts = exifParserStart(exifGetThumbInfo, &thumbPara, pBufAddrWord, decSize);
			//sts = exifParsingFile(pBufAddrWord, decSize, 0/*bShowThumb*/, pFile->MediaTime, &imagePara,&thumbPara);
			//wangjg++ 20090107
			if(exifAppN)
			{
				UINT8 xdata qTable[128];
				//jpegQTableSet(85, NULL, NULL, 1); //see file:snap_task.c line:1405
				SDRAM_Dram2Sram(exifQvYTblAddr + exifQvVlcAtOdd,qTable,128);
				jpegQTableSet(0, qTable, (UINT8*)(qTable+64), 1);
			
			}
			//wangjg--	20090107	
		}
		//NeedReadMoreHeader = 0;
		pPara = &imagePara;
		decPara.ImgType = 0; //main image
	}
	//xian --

	vfsFileClose(fHandle,  pFile->FileName, K_DOS_ReadFile, 0);
	if (sts != TRUE) {
		DBG_pbFuncTrack();
		return FAIL;
	}

	pFile->MediaWidth = imagePara.width;
	pFile->MediaHeight = imagePara.height;
//	printf("w is %u, h is %u \n", ImagePara.Width, ImagePara.Height);
#if (DBG_Level > 1)
	printf("Exif w %u, h %u \n", thumbPara.width, thumbPara.height);
#endif //(DBG_Level > 1)

	//Decompress
	//xian ++ 20080618
	pbParamSet(PB_PARAM_EXIF_ORIENT, exifRot);	// param is UINT16
	rotMode = pbDecRotModeGet();
#if (DBG_Level > 1)
	printf("rotMode=%bu\n", rotMode);
#endif //(DBG_Level > 1)
	{
		decPara.DecBuff = K_SDRAM_ImagePlaybackBufAddr;
		decPara.RotMode = rotMode;
		decPara.DecW = pTumbLayout->ImgWidth;
		decPara.DecH = pTumbLayout->ImgHeight;
		//decPara.ImgType = ImagePara.DataType; // 1;//Type thumb image

		pbImgDeCmpRot(pPara/*&ThumbPara*/, &decPara, 1);

		//xian ++ 20080703
		if(pTumbLayout->VoiceMemo == TRUE){
			pbDrawIconType(K_SDRAM_ImagePlaybackBufAddr,
							//ThumbPara.Width, ThumbPara.Height,
							decPara.DecW, decPara.DecH,
							PB_PARAM_MT_MEMO,
							//K_SDRAM_ImagePlaybackBufAddr + ThumbPara.Width * ThumbPara.Height);
							K_SDRAM_ImagePlaybackBufAddr + decPara.DecW * decPara.DecH);
		}
		if((pTumbLayout->pImgInfo.FileAttrib & 0x03) == 1){
			pbDrawIconType(K_SDRAM_ImagePlaybackBufAddr,
							//ThumbPara.Width, ThumbPara.Height,
							decPara.DecW, decPara.DecH,
							PB_PARAM_MT_LOCK,
							//K_SDRAM_ImagePlaybackBufAddr + ThumbPara.Width * ThumbPara.Height);
							K_SDRAM_ImagePlaybackBufAddr + decPara.DecW * decPara.DecH);
		}
		//xian --

		HAL_GprmCopy(
						K_SDRAM_ImagePlaybackBufAddr,
						//ThumbPara.Width, ThumbPara.Height, 0, 0,
						//decPara.DecW, decPara.DecH, 0, 0,
						pTumbLayout->ImgWidth,pTumbLayout->ImgHeight,0,0,//80,64,0,0, cx modify
						pbDispBuffGet(1),
						decWidth, decHeight, pTumbLayout->OffsetX, pTumbLayout->OffsetY,
						pTumbLayout->ImgWidth, pTumbLayout->ImgHeight,
						0,
						1
		);

		multiPbPos[multiIndex].x0 =pTumbLayout->OffsetX;
		multiPbPos[multiIndex].y0 =pTumbLayout->OffsetY;
		multiPbPos[multiIndex].w =pTumbLayout->ImgWidth;
		multiPbPos[multiIndex].h =pTumbLayout->ImgHeight;

		//printf("thumx=%d,%d,%d,%d\n",pTumbLayout->OffsetX, pTumbLayout->OffsetY,pTumbLayout->ImgWidth, pTumbLayout->ImgHeight);
		
	}
	//xian --
	HAL_GlobalCamModeSet(CAM_MODE_IDLE);

	return SUCCESS;
}
#endif
//-----------------------------------------------------------------------------
//pbMultiPlay
//-----------------------------------------------------------------------------
/**
 * @fn		void pbMultiPlay(void)
 * @brief		Multiply Play
 * @param     NONE
 * @retval	NONE
 * @see		NULL
 * @author    Tq.Zhang
 * @since	2010-5-5
 * @todo		N/A
 * @bug		N/A
*/
#if STILL_LIB_PB_DMY
void pbMultiPlay(void) USING_0
{
}
#else
void pbMultiPlay(void) USING_0
{
	UINT8 i;
	UINT8 time[20] = {0};
	PBMultiImg_t xdata* pMpImgObj = &pbMpObj[0];
	UINT16 decWidth;
	UINT16 decHeight;
	UINT16 cnt=1;
	UINT8 dev;
	
	multiPlayEn =1;
	osSysModeSet(OS_SYS_MODE_DEFAULT);
	sysMemStillPbDispAddrUpdate(dispDevActGet() ==4);
	dev = dispDevActGet();
	switch(dev){
		case 1:		//LCD
			 decWidth  = PB_DECODE_WIDTH;
	 		 decHeight = PB_DECODE_HEIGHT;
		break;
		case 2:		//NTSC
			 decWidth = 720;
	 		 decHeight = 480;	
		break;
		case 3:		//PAL
			 decWidth = (UINT16)720;
	 		 decHeight = (UINT16)576;
		break;
		case 4:		//HDMI
			 decWidth = HDMI_BUF_W;
	 		 decHeight = HDMI_BUF_H;
		break; 
	}
	//xian ++ 20110214
	pbDispQVSizeSet(decWidth, decHeight);
	//xian --
	pbDispBuffSwitch();

	//dbgTimerStart();
	gprmBlackFill(pbDispBuffGet(1), decWidth, decHeight);
	//fill black to display buffer first
	
	//dbgTimerStop();
	dispShow(pbDispBuffGet(1), decWidth, decHeight, 100);
	multiIndex = 0;
	//printf("disp add is 0x%08lx\n", pbDispBuffGet(1));
	for (i = 0; i < pbMpLayout.Max; i++) {
	
		multiIndex=i;
		multiPbPos[multiIndex].x0 =0;
		multiPbPos[multiIndex].y0 =0;
		multiPbPos[multiIndex].w =0;
		multiPbPos[multiIndex].h =0;
		
		if (pMpImgObj->err == SUCCESS) {
			if (pMpImgObj->pImgInfo.FileType == SP1K_PB_FILE_TYPE_JPG) {
				//CPrintf("JPG");
				pMpImgObj->err = pbMultiPlayDecode(
				//&pMpImgObj->pImgInfo,
				K_SDRAM_ImagePlaybackGeneralBufAddr,
				pMpImgObj,
				PB_DECODE_OPT_THUMB
				);
			} 
			else if ((pMpImgObj->pImgInfo.FileType == SP1K_PB_FILE_TYPE_AVI)||(pMpImgObj->pImgInfo.FileType == SP1K_PB_FILE_TYPE_WGI)) {
				pMpImgObj->err = pbMultiPlayVideoFrameDecode(pMpImgObj);
				//CPrintf("AVI");
				//pMpImgObj->err = SUCCESS;
			}
			else if (pMpImgObj->pImgInfo.FileType == SP1K_PB_FILE_TYPE_WAV) {
				//CPrintf("WAV");
				pMpImgObj->err = pbMultiPlayAudioBgDecode(pMpImgObj);
				//pMpImgObj->err = SUCCESS;
			} 
			else {
				pMpImgObj->err = FAIL;
			}

			//xian ++, 20080716
			if(pMpImgObj->err == FAIL){
				pbMultiPlayError((UINT32)pbParamGet(PB_PARAM_FILE_ERR_RES), pMpImgObj);
			}
			//xian --

			if ((i + 1) == pbMpActiveObjIdx) {
				/*add by phil for update active obj information s*/
				memcpy(&MediaInfo, &((pMpImgObj + pbMpActiveObjIdx)->pImgInfo), sizeof(PBFile_t));
				/*add by phil for update active obj information e*/
				pbMultiPlayObjFrameDraw(pbMpActiveObjIdx, 1);
			}
		} else {
			//printf("Tmb %bu error\n", i);
		}
		pMpImgObj++;
	}
		
	multiPlayEn =0;
	//return ;
}
#endif
//-----------------------------------------------------------------------------
//pbMultiPlayShift
//-----------------------------------------------------------------------------
/**
 * @fn		UINT8 pbMultiPlayShift(UINT16* fileIdx, UINT8 shift)
 * @brief		pbMultiPlayShift
 * @param     [in] fileIdx
 * @param     [in] shift
 * @retval	return = SUCCESS / FAIL
 * @see		NULL
 * @author    Tq.Zhang
 * @since	2010-5-5
 * @todo		N/A
 * @bug		N/A
*/
#if STILL_LIB_PB_DMY
UINT8 pbMultiPlayShift(UINT16* fileIdx, UINT8 shift)
{
        fileIdx=fileIdx;
        shift=shift;
        return SUCCESS;
}
#else
UINT8 pbMultiPlayShift(UINT16* fileIdx, UINT8 shift)
{
	UINT16 fileTotal;
	UINT16 newFileIdx, oldPage, newPage;
	PBMultiImg_t xdata* pMpImgObj = &pbMpObj[0];

	fileTotal = dcfFileCountGet();

	//cal newFileIdx
	switch(shift){
		case PB_SHIFT_UP:
			if(*fileIdx > pbMpLayout.HNum){
				newFileIdx = *fileIdx - pbMpLayout.HNum;
			}
			else{//goto last line
				newFileIdx = (fileTotal - 1) / pbMpLayout.HNum * pbMpLayout.HNum + *fileIdx;
				if(newFileIdx > fileTotal){
					newFileIdx = newFileIdx - pbMpLayout.HNum;	//up a line
					if ((newFileIdx - 1 )/pbMpLayout.Max != (fileTotal - 1)/pbMpLayout.Max){
						//new is not last page
						newFileIdx = fileTotal;
					}
				}
			}
			break;

		case PB_SHIFT_DOWN:
			newFileIdx = *fileIdx + pbMpLayout.HNum;
			if(newFileIdx > fileTotal){
				if ((*fileIdx - 1 )/pbMpLayout.Max != (fileTotal - 1)/pbMpLayout.Max){
					//old is not last page
					newFileIdx = fileTotal;
				}
				else{//goto first line
					newFileIdx = newFileIdx % pbMpLayout.HNum;
					if(newFileIdx == 0){
						newFileIdx = pbMpLayout.HNum;
					}
				}
			}
			break;

		case PB_SHIFT_LEFT:
			newFileIdx = *fileIdx - 1;
			if(newFileIdx == 0){
				newFileIdx = fileTotal;
			}
			break;

		case PB_SHIFT_RIGHT:
			newFileIdx = *fileIdx + 1;
			if(newFileIdx > fileTotal){
				newFileIdx = 1;
			}
			break;
		case PB_SHIFT_PAGE_UP:
			if(*fileIdx >9)
			{
				newFileIdx = *fileIdx - 9;
			}
			else
			{
				newFileIdx = fileTotal;
			}
			break;
		case PB_SHIFT_PAGE_DOWN:
			newFileIdx = *fileIdx + 9;
			if(newFileIdx > fileTotal)
			{
				newFileIdx = 1;
			}
			break;
		default:
			return FAIL;
	}

	//printf("old idx = %u, %bu\n", *fileIdx, pbMpActiveObjIdx);
	pbMpActiveObjIdx = newFileIdx % pbMpLayout.Max;
	if(pbMpActiveObjIdx == 0){
		pbMpActiveObjIdx = pbMpLayout.Max;
	}
	//printf("new idx = %u, %bu\n", newFileIdx, pbMpActiveObjIdx);

	 /*add by phil for update active obj information s*/
	pMpImgObj += pbMpActiveObjIdx;
	memcpy(&MediaInfo, &(pMpImgObj->pImgInfo), sizeof(PBFile_t));
	/*add by phil for update active obj information e*/

	//need change page?
	oldPage = (*fileIdx - 1 )/pbMpLayout.Max;
	newPage = (newFileIdx - 1)/pbMpLayout.Max;
	*fileIdx = newFileIdx;
	if(newPage == oldPage)
	{
		//printf("same page : %u\n", newPage);
		pbMultiPlayObjFrameDraw(pbMpActiveObjIdx, 1);
	}
	else
	{
		return FAIL;
		//printf("change page : %u -> %u\n", oldPage, newPage);
		//pbMultiPlayLayoutInit(pbMpLayout.Type, *fileIdx);
		//pbMultiPlay();
	}
	return SUCCESS;
}
#endif
//-----------------------------------------------------------------------------
//pbMultiPlayObjFrameDraw
//-----------------------------------------------------------------------------
/**
 * @fn		void pbMultiPlayObjFrameDraw(UINT16 idx, UINT8 enable)
 * @brief		pbMultiPlayObjFrameDraw
 * @param     [in] idx
 * @param     [in] enable
 * @retval	NONE
 * @see		NULL
 * @author    Tq.Zhang
 * @since     2010-5-5
 * @todo      N/A
 * @bug       N/A
*/
#if STILL_LIB_PB_DMY
void pbMultiPlayObjFrameDraw(UINT16 idx, UINT8 enable)
{
        idx=idx;
        enable=enable;
}
#else
void pbMultiPlayObjFrameDraw(UINT16 idx, UINT8 enable)
{
	PBMultiImg_t xdata* pMpImgObj;
	UINT16 dispH, dispW;
	UINT16 decHeight, decWidth;
	UINT16 tmpX,tmpY;
	UINT8 ratX,ratY;
	
	if (!idx) {
		return;
	}

	//dispPnlDimGet(&dispH, &dispW);
	osdDispBuffSizeGet(&dispW, &dispH);
	pbDispQVSizeGet(&decWidth, &decHeight);	//xian 20110214
	idx = idx % pbMpLayout.Max;
	if (!idx) {
		idx = pbMpLayout.Max;
	}
	pMpImgObj = &pbMpObj[idx - 1];
	if (enable) {
		tmpX=((idx-1)%pbMpLayout.HNum+1);
		tmpY=((idx-1)/pbMpLayout.VNum+1);		
		if(1==dispDevActGet()){//LCD
			UINT16 tmpParaX=0;
			UINT16 tmpParaY=0;
			UINT16 panelH,panelV;
			
			dispPnlDimGet(&panelV, &panelH);
			dispPnlAspectRatioGet(&ratX, &ratY);
			if(panelH==400 &&panelV==240&& (4*ratY <3*ratX)){//txd3.0 16:9(400*240)
				dispW=260;
				dispH=240;
				tmpParaX =30+(12-6*tmpX);
			}else if(panelH==960 &&panelV==240 && (4*ratY <3*ratX)){//txd2.7 UPS051 16:9(960*240)
				dispW=240;
				dispH=240;
				tmpParaX=40;
			}else{ 
				if(panelH == 220 || panelV==176){//220*176    
					dispW=panelH+1;
					dispH=panelV+1;	
					tmpParaX =2-tmpX;
					tmpParaY =(tmpY==3)?(-1):0;
				}else{ //160*128
					dispW=panelH+1;
					dispH=panelV+2;
					//tmpParaX =(tmpX==2)?0:1;
					tmpParaY =(2-tmpY);
					if(tmpY==1)
					{
						tmpParaY--;
					}
				}
			}	
			osdSelWinSet((UINT32)((UINT32)pMpImgObj->OffsetX * (UINT32)dispW) / (UINT32)decWidth+tmpParaX,
						(UINT32)((UINT32)pMpImgObj->OffsetY * (UINT32)dispH) / (UINT32)decHeight+tmpParaY,
						(UINT32)((UINT32)pMpImgObj->ImgWidth * (UINT32)dispW) / (UINT32)decWidth,
						(UINT32)((UINT32)pMpImgObj->ImgHeight * (UINT32)dispH) / (UINT32)decHeight,
						2,
						0x3e
			);
		}else if(4==dispDevActGet()){//HDMI
			osdSelWinSet((UINT32)((UINT32)(pMpImgObj->OffsetX +(90*tmpX))* (UINT32)dispW) / (UINT32)decWidth -49* (tmpX-1),
						(UINT32)((UINT32)(pMpImgObj->OffsetY -(16*tmpY))* (UINT32)dispH) / (UINT32)decHeight +10* (tmpY-1),
						(UINT32)((UINT32)(pMpImgObj->ImgWidth - 40)* (UINT32)dispW) / (UINT32)decWidth ,
						(UINT32)((UINT32)(pMpImgObj->ImgHeight +20)* (UINT32)dispH) / (UINT32)decHeight,
						2,
						0x3e
			);	
		}else if(2==dispDevActGet()){//TV NTSC OUT
			osdSelWinSet((UINT32)((UINT32)(pMpImgObj->OffsetX -(33*tmpX))* (UINT32)dispW) / (UINT32)decWidth +25* (tmpX-1),
						(UINT32)((UINT32)(pMpImgObj->OffsetY +(4*tmpY))* (UINT32)dispH) / (UINT32)decHeight -10* (tmpY-1),
						(UINT32)((UINT32)(pMpImgObj->ImgWidth + 40 )* (UINT32)dispW) / (UINT32)decWidth ,
						(UINT32)((UINT32)(pMpImgObj->ImgHeight +6)* (UINT32)dispH) / (UINT32)decHeight,
						2,
						0x3e
			);	
			
		}else {//TV PAL OUT
			osdSelWinSet((UINT32)((UINT32)(pMpImgObj->OffsetX -(25*tmpX))* (UINT32)dispW) / (UINT32)decWidth +16* (tmpX-1),
						(UINT32)((UINT32)(pMpImgObj->OffsetY -(56*tmpY))* (UINT32)dispH) / (UINT32)decHeight +35* (tmpY-1),
						(UINT32)((UINT32)(pMpImgObj->ImgWidth + 40 )* (UINT32)dispW) / (UINT32)decWidth ,
						(UINT32)((UINT32)(pMpImgObj->ImgHeight +44)* (UINT32)dispH) / (UINT32)decHeight,
						2,
						0x3e
			);	
			
		}
	} else {
		osdSelWinSet(-1, -1, -1, -1, 0, -1);
	}

	//printf("X %u, Y %u", pMpImgObj->OffsetX, pMpImgObj->OffsetY);
	osdRegUpdate();
}
#endif
//-----------------------------------------------------------------------------
//pbMultiPlayAudioBgDecode
//-----------------------------------------------------------------------------
/**
 * @fn		UINT8 pbMultiPlayAudioBgDecode(PBMultiImg_t* pTumbLayout)
 * @brief		pbMultiPlayAudioBgDecode
 * @param	pTumbLayout
 * @retval	SUCCESS / FAIL
 * @see		NONE
 * @author	Phil Lin
 * @since	2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
#if (!STILL_LIB_PB_DMY)
static UINT8 pbMultiPlayAudioBgDecode(PBMultiImg_t* pTumbLayout)
{
	UINT8 status;
	UINT8 ret, sts;
	UINT32 BuffBtm = K_SDRAM_TotalSize - 256UL;
	UINT32 BuffTop = K_SDRAM_ImagePlaybackBufAddr;
	UINT32 ResSize;
	UINT32 SrcAddr, DstAddr;
	UINT16 decHeight, decWidth;
	UINT16 imgScaleFac;
	UINT16 srcWidth = 320;
	UINT16 srcHeight = 240;
	UINT32 size;
	//struct Exif_Decompress_Parameter MainImgPara;
	ExifImage_t mainImage;
	UINT8 opt;

	if (!pTumbLayout) {
		return FAIL;
	}

	//Load resource file
	ENTER_CRITICAL(status);
	sp1kNandRsvSizeGet(pbParam[PB_PARAM_AUDIO_RES_ID], NULL, &ResSize);
	SrcAddr = BuffBtm - ResSize;
	DstAddr = BuffTop;
	ret = sp1kNandRsvRead(pbParam[PB_PARAM_AUDIO_RES_ID], SrcAddr << 1);
	EXIT_CRITICAL(status);
	if (ret != TRUE) {
		return FAIL;
	}

	//calculate scale fact
	imgScaleFac = 0x00;
	if (srcWidth > pTumbLayout->ImgWidth) {
		while (((srcWidth >> 3) * ++imgScaleFac) <= pTumbLayout->ImgWidth);
		imgScaleFac--;// decopress width < PB_DECODE_WIDTH
	}

	/* exif header parsing */
	sp1kNandRsvSizeGet(pbParam[PB_PARAM_AUDIO_RES_ID], NULL, &size);
	sts = exifParserStart(exifGetMainInfo, &mainImage, SrcAddr, size);
	//sts = exifParsingFile(SrcAddr, size, 0, NULL, &MainImgPara, NULL);
	if (sts != TRUE) {
		//printf("exifParsingFile Fail\n");
		return FAIL;
	}
	
	/* check YUV format and if the vlc data is at odd address */
	if (mainImage.dataType == K_File_DataType_YUV420) {
		opt = 0x05;
	} else if (mainImage.dataType == K_File_DataType_YUV422) {
		opt = 0x04;
	}
	if (exifVlcAtOdd) {
		exifVlcAtOdd = 0;
		opt |= 0x10;
	}

	/* image decode process */
	HAL_JpegDecode(
					mainImage.addr,
					mainImage.width,
					mainImage.height,
					DstAddr,
					imgScaleFac,
					opt,
					G_JpgRstMcuNum
	);

	//calculate destination size
	if (imgScaleFac) {
		srcWidth = (((srcWidth * (UINT16)imgScaleFac) >> 3) + 7) &~7;
		srcHeight = (srcHeight * (UINT16)imgScaleFac) >> 3;
	}
	SrcAddr = DstAddr;

	//scale to fitted size
	HAL_GprmScale(
					SrcAddr,
					srcWidth, srcHeight,
					pbDispBuffGet(0),
					pTumbLayout->ImgWidth, pTumbLayout->ImgHeight,
					SrcAddr + (UINT32)srcWidth * (UINT32)srcHeight,
					1
	);

	//xian ++ 20080703
	if((pTumbLayout->pImgInfo.FileAttrib & 0x03) == 1){
		pbDrawIconType(pbDispBuffGet(0),
						pTumbLayout->ImgWidth, pTumbLayout->ImgHeight,
						PB_PARAM_MT_LOCK,
						pbDispBuffGet(0) + pTumbLayout->ImgWidth * pTumbLayout->ImgHeight);
	}
	//xian --

	//Copy to display buffer
	pbDispQVSizeGet(&decWidth, &decHeight);	//xian 20110214
	HAL_GprmCopy(
					pbDispBuffGet(0),
					pTumbLayout->ImgWidth, pTumbLayout->ImgHeight,
					0, 0,
					pbDispBuffGet(1),
					decWidth, decHeight,
					pTumbLayout->OffsetX, pTumbLayout->OffsetY,
					pTumbLayout->ImgWidth, pTumbLayout->ImgHeight,
					0,
					1
	);
	multiPbPos[multiIndex].x0 =pTumbLayout->OffsetX;
	multiPbPos[multiIndex].y0 =pTumbLayout->OffsetY;
	multiPbPos[multiIndex].w =pTumbLayout->ImgWidth;
	multiPbPos[multiIndex].h =pTumbLayout->ImgHeight;

	//printf("thumx=%d,%d,%d,%d\n",pTumbLayout->OffsetX, pTumbLayout->OffsetY,pTumbLayout->ImgWidth, pTumbLayout->ImgHeight);

	return SUCCESS;

}

//-----------------------------------------------------------------------------
//pbMultiPlayVideoFrameDecode
//-----------------------------------------------------------------------------
/**
 * @brief		pbMultiPlayVideoFrameDecode
 * @param	pTumbLayout
 * @retval	SUCCESS / FAIL
 * @see		NONE
 * @author	Phil Lin
 * @since		2008-02-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/

static UINT8 pbMultiPlayVideoFrameDecode(PBMultiImg_t* pTumbLayout)
{
	UINT16 FileIndex;  //xian ++ 20080703
	UINT32 backAddr = K_SDRAM_ImagePlaybackBufAddr;
	UINT16 decWidth;
	UINT16 decHeight;
	UINT16 disWidth;
	UINT16 disHeight;
	UINT8 sts ;
	
	FileIndex = pTumbLayout->pImgInfo.FileIdx; 
	decWidth = pTumbLayout->ImgWidth;
	decHeight = pTumbLayout->ImgHeight;
	
	sts = pbVideoFrameDecode(FileIndex, decWidth, decHeight, backAddr, 1);    
	if(sts == SUCCESS){
		pbDrawIconType(backAddr,
						pTumbLayout->ImgWidth, pTumbLayout->ImgHeight,
						PB_PARAM_MT_VIDEO,
						backAddr + pTumbLayout->ImgWidth * pTumbLayout->ImgHeight);
		if((pTumbLayout->pImgInfo.FileAttrib & 0x03) == 1){
			pbDrawIconType(backAddr,
							pTumbLayout->ImgWidth, pTumbLayout->ImgHeight,
							PB_PARAM_MT_LOCK,
							backAddr + pTumbLayout->ImgWidth * pTumbLayout->ImgHeight);
		}

		//Copy to display buffer
		pbDispQVSizeGet(&disWidth, &disHeight);
		HAL_GprmCopy(
						backAddr,
						pTumbLayout->ImgWidth, pTumbLayout->ImgHeight,
						0, 0,
						pbDispBuffGet(1),
						disWidth, disHeight,
						pTumbLayout->OffsetX, pTumbLayout->OffsetY,
						pTumbLayout->ImgWidth, pTumbLayout->ImgHeight,
						0,
						1
		);
		multiPbPos[multiIndex].x0 =pTumbLayout->OffsetX;
		multiPbPos[multiIndex].y0 =pTumbLayout->OffsetY;
		multiPbPos[multiIndex].w =pTumbLayout->ImgWidth;
		multiPbPos[multiIndex].h =pTumbLayout->ImgHeight;

		//printf("thumx=%d,%d,%d,%d\n",pTumbLayout->OffsetX, pTumbLayout->OffsetY,pTumbLayout->ImgWidth, pTumbLayout->ImgHeight);

		
	}

	return sts;
}

static UINT8 pbLoadIcon(UINT32 addr, UINT8 iconParId, pbIconInfo_t *iconInfo)
{
	UINT8 ret = FALSE;

	if(!iconInfo || !addr)
		return ret;

	switch(iconParId){
		case PB_PARAM_MT_LOCK:
		case PB_PARAM_MT_MEMO:
		case PB_PARAM_MT_VIDEO:
			break;
		default:
			return ret;
	}

	iconInfo->resId = (UINT8)(pbParamGet(iconParId) & PB_PARAM_MT_RESID_MASK);
	iconInfo->aligFmt = (UINT8)((pbParamGet(iconParId) & PB_PARAM_MT_ALIG_MASK) >> 8);
	iconInfo->addr = addr;
	iconInfo->w = PB_MT_ICON_W;
	iconInfo->h = PB_MT_ICON_H;

	ret = sp1kNandRsvRead((UINT32)iconInfo->resId, (iconInfo->addr) << 1);
	return ret;
}

UINT8 pbDrawIconType(UINT32 imgBuf, UINT16 imgW, UINT16 imgH, UINT8 iconParId, UINT32 tmpBuf )
{
	UINT8 ret;
	pbIconInfo_t icon;
	UINT16 offX, offY;

	//load
	ret = pbLoadIcon(tmpBuf, iconParId, &icon);
	if(ret == FALSE)
		return ret;

	//offset
	offX = 0;	//default left and top
	offY = 0;

	if(icon.aligFmt & (UINT8)(PB_ICON_RIGHT >> 8)){
		offX = imgW - icon.w - 1;
	}
	else if(icon.aligFmt & (UINT8)(PB_ICON_CENTER >> 8)){
		offX = (imgW - icon.w) >> 1;
	}

	if(icon.aligFmt & (UINT8)(PB_ICON_BOTTOM >> 8)){
		offY = imgH - icon.h - 1;
	}
	else if(icon.aligFmt & (UINT8)(PB_ICON_VCENTER >> 8)){
		offY = (imgH - icon.h) >> 1;
	}
	offX &= ~1;

	//draw
	HAL_GprmCopy(
					icon.addr,
					icon.w, icon.h, 0, 0,
					imgBuf, imgW, imgH,	offX, offY,
					icon.w, icon.h,
					0x81,
					1
	);

}

static UINT8 pbMultiPlayError(UINT32 resId, PBMultiImg_t* pTumbLayout) USING_0
{
	UINT16 decWidth, decHeight;
	UINT32 decAddr, tmpAddr;
	UINT32 bufAddr = K_SDRAM_ImagePlaybackGeneralBufAddr;

	if(sp1kNandRsvRead(resId, bufAddr << 1)){
		pbDispQVSizeGet(&decWidth, &decHeight);

		decAddr = bufAddr + (UINT32)PB_FILE_ERROR_W * (UINT32)PB_FILE_ERROR_H;
		tmpAddr = decAddr + (UINT32)PB_FILE_ERROR_W * (UINT32)PB_FILE_ERROR_H;

		HAL_GprmScale(
						bufAddr,
						PB_FILE_ERROR_W, PB_FILE_ERROR_H,
						decAddr,
						pTumbLayout->ImgWidth, pTumbLayout->ImgHeight,
						tmpAddr,
						1
		);

		HAL_GprmCopy(
						decAddr,
						pTumbLayout->ImgWidth, pTumbLayout->ImgHeight, 0, 0,
						pbDispBuffGet(1),
						decWidth, decHeight, pTumbLayout->OffsetX, pTumbLayout->OffsetY,
						pTumbLayout->ImgWidth, pTumbLayout->ImgHeight,
						0,
						1
		);
	}
	return SUCCESS;
}
#endif

multiPbSelWin_t * pbMultiPosGet(void)
{
	return multiPbPos;
}
