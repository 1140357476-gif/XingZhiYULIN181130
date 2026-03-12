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
 * @file      pb_edit.c
 * @brief     decode and edit picture
 * @author    tq.zhang
 * @since     2010-05-06
 * @date      2010-05-06
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "initio.h"
#include "pb_general.h"
#include "pb_api.h"
#include "pb_debug.h"
//#include "exifhdr.h"
#include "exiffunc.h"
#include "doslink.h"
#include "cardlink.h"
#include "hal.h"
#include "sp1k_cdsp_api.h"
#include "hal_cdsp_iq.h"
#include "hal_jpeg.h"
#include "sp1k_snap_api.h"
#include "snap_qtable.h"
#include "fileindex.h"
#include "disp_api.h"
#include "spe_distort.h"
#include "pb_photoframe.h"
#include "Sp1k_rsvblk_api.h"
#include "hal_gprm.h"
#include "cdsp_task.h"
#include "utility.h"
#include "vfs_api.h"
#include "dcf_api.h"
#include "snap_core.h"
#include "hal_dram.h"
#include "sp1k_pb_api.h"
#include "dos32.h"

#include "exif_entry.h"
#include "exif_param.h"
#include "hal_disp.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#if (!STILL_LIB_PB_DMY)
#define PB_EFFECT_DECODE_WIDTH 	(1024UL)//(2048UL)
#define PB_EFFECT_DECODE_HEIGHT 	(768UL)//(1536UL)
#define PB_EFFECT_DECODE_MAX		(PB_EFFECT_DECODE_WIDTH * PB_EFFECT_DECODE_HEIGHT) // 3M
#define OrderToAddr(addr, offset)		((addr) = ((addr) + ((offset) - 1)) &~ ((offset) - 1))

#define PB_EDIT_IMG_QTAB			70//90 /*will revise */	
#define PB_EDIT_IMG_TUMB_W		160
#define PB_EDIT_IMG_TUMB_H		120
#define PB_EDIT_IMG_HDR_SIZE	(64UL * 1024UL)

#define PB_EDIT_CDSP_DO_LINE_NUM	32UL

#define DEBUG_YUV		0

#define DEBUG_PRINT     //printf

#define MCU_ALIGN(Data,Align_16) ((Data) = (Data) &~ ((Align_16) - 1))
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
//extern UINT8 cdspYuv2Yuv(UINT32 srcAddr,UINT32 dstAddr,UINT16 xSize,UINT16 ySize,UINT16 doYSize);
//extern void gprmBlackFill(UINT32 dstAddr, UINT16 imgW, UINT16 imgH);
extern xdata UINT8 exifVlcAtOdd;

extern xdata UINT16 pbParam[PB_PARAM_MAX];
extern xdata UINT8 exifFillRotOpt;
extern xdata UINT16 G_JpgRstMcuNum;

extern xdata UINT8 exifAppN;
extern xdata UINT8 exifQvVlcAtOdd;
extern xdata UINT32 exifQvUVTblAddr;
extern xdata UINT8 exifQvYTblAddr;
extern PBFile_t xdata MediaInfo;

UINT8 Rotation_En = 0; 
UINT8 cropFlag = 0;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
static void pbCdspEffectDo(pbEditType_t imgEffect, UINT32 srcAddr, UINT16 srcWidth, UINT16 srcHeight, UINT32 dstAddr , UINT16 dstWidth, UINT16 dstHeight);
//static void pbCdspEffectDo(PBEffectList_t imgEffect, UINT32 srcAddr, UINT16 srcWidth, UINT16 srcHeight, UINT32 dstAddr , UINT16 dstWidth, UINT16 dstHeight);
UINT8 pbEditFileMake(UINT32 startAddr, UINT32 size);
//UINT8 pbEffect_Resize(PBFile_t* pFile, PBEffectList_t imgEffect, UINT16 dstWidth, UINT16 dstHeight);
//=============================================================================
//Program
//=============================================================================
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
/**
 * @fn        void pbCdspEffectDo(PBEffectList_t imgEffect, UINT32 srcAddr, UINT16 srcWidth, UINT16 srcHeight, UINT32 dstAddr)
 * @brief     pbCdspEffectDo
 * @param     [in] imgEffect : effect type: NORMAL/RED/GREEN/BLUE/BINARIZE/SEPIA/SUNSET/WARM/COLD/BLACKWHITE/CONTRAST
 * @param     [in] srcAddrWord : source YUV address
 * @param     [in] srcWidth : source pixel width
 * @param     [in] srcHeight : source pixel height
 * @param     [in] dstAddrWord : destination address
 * @retval    NONE
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-6
 * @todo      N/A
 * @bug       N/A
*/
static void 
pbCdspEffectDo(
	pbEditType_t imgEffect,
	UINT32 srcAddrWord, 
	UINT16 srcWidth, 
	UINT16 srcHeight, 
	UINT32 dstAddrWord,
	UINT16 dstWidth,
	UINT16 dstHeight
)
{

	//HAL_CdspEdgeCoringMedianEnSet( 0, 0, 0, 0);
	//HAL_CdspYEdgeEnSet(0);
	//HAL_CdspLEdgeEnSet(0);
	//HAL_CdspYCoringEnSet(0);
	//HAL_CdspUVMedianEnSet(0);
		
	//HAL_CdspYuvFilterBufSet(0);
	
	cdspPostProcessCtrl(0,CDSP_DO_YUV2YUV);
	
	switch (imgEffect) {
		case PB_IMG_EFFECT_NORMAL:
			cdspNormalEffectSet();
			break;
		case PB_IMG_EFFECT_RED:
			cdspRedEffectSet();
			break;
		case PB_IMG_EFFECT_GREEN:
			cdspGreenEffectSet();
			break;
		case PB_IMG_EFFECT_BLUE:
			cdspBlueEffectSet();
			break;
		case PB_IMG_EFFECT_BINARIZE:
			cdspBinarizeEffectSet();
			break;
		case PB_IMG_EFFECT_NEGTIVE:
			cdspNegativeEffectSet();
			break;
		case PB_IMG_EFFECT_SEPIA:
			cdspSepiaEffectSet();
			break;
		case PB_IMG_EFFECT_SUNSET:
			cdspWarmEffectSet();
			break;
		case PB_IMG_EFFECT_WARM:
			cdspWarmEffectSet();
			break;
		case PB_IMG_EFFECT_COLD:
			cdspColdEffectSet();
			break;
		case PB_IMG_EFFECT_BLACKWHITE:
			cdspBlackWhiteEffectSet();
			break;
		case PB_IMG_EFFECT_CONTRAST:
			cdspContrastEffectSet();
			break;
		default:
			//printf("UnSupport format!\n");
			break;
	}
	cdspYuv2Yuv( srcAddrWord, dstAddrWord, srcWidth, srcHeight, dstWidth, dstHeight, PB_EDIT_CDSP_DO_LINE_NUM);
	cdspNormalEffectSet();

}

/**
 * @fn        UINT8 pbEditFileMake(UINT32 startAddr, UINT32 size)
 * @brief     save edit file
 * @param     [in] startAddrWord : source data word address
 * @param     [in] size : data size
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-6
 * @todo      N/A
 * @bug       N/A
*/
UINT8 pbEditFileMake(UINT32 startAddrWord, UINT32 size)
{
	UINT8 sts;
	UINT8 fileName[13];

	sts = dcfFileIndexUpdate();   //xian ++ 20081203
	if(sts != TRUE){
		return FAIL;
	}
	//dcfJpgNameGet(fileName, dcfFileIndexGet());
	dcfMediaNameGet(fileName, dcfFileIndexGet(),MEDIA_JPG);
	fileName[12] = 0x00;	
	//G_USR_NewFileSizeInFdb = size;

	sts = dramFileSave( fileName, startAddrWord, size);

	if (sts != TRUE) {
		DBG_pbFuncTrack();
		return FAIL;
	}
#if 1
	dcfNewFileDramFdbParameterUpdate(K_DCF_JPG_TYPE, size);
#else
	G_USR_NewFileSizeInFdb = size;
	dcfFileDramFdbParameterWrite(K_DCF_JPG_TYPE,G_USR_FileIndex);
	G_USR_LastFileIndex = G_USR_FileIndex; //yichang@0510

	//Fix file index bug begin
	if (G_USR_FileCount == 0)
	{
		G_USR_FirstFileIndex = G_USR_FileIndex;
	}
	//Fix file index bug end
	
	G_USR_FileIndex ++;
	G_USR_FileCount++;
	DOS_FileOrderWrite(G_USR_FileCount , G_USR_LastFileIndex);
#endif
	return SUCCESS;
}
#endif

//xian ++ 20080530
/**
 * @fn        UINT32 pbUserStartImgGet(UINT32 *vlcAddrWord)
 * @brief     pbUserStartImgGet
 * @param     [in] vlcAddrWord : 
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-6
 * @todo      N/A
 * @bug       N/A
*/
#if STILL_LIB_PB_DMY
UINT32 pbUserStartImgGet(UINT32 *vlcAddrWord)
{
        vlcAddrWord=vlcAddrWord;
        return 0;
}
#else
UINT32 pbUserStartImgGet(UINT32 *vlcAddrWord)
{
	UINT32 dramEndAddr = K_SDRAM_TotalSize - 256UL;//512 Bytes reserved for DMA
	UINT32 freeBuffAddr = K_SDRAM_ImagePlaybackBufAddr;
	UINT32 tempBuff =K_SDRAM_TotalSize-(UINT32)320*240*2;
	UINT32 scalBuff = tempBuff - (UINT32)320*240;
	UINT32 vlcSize = 0;
	UINT32 srcAddr;
	UINT16 dstW, dstH, decW, decH;
	UINT32 size = 0;
	UINT16 srcWidth,srcHeight;
	UINT32 dispAddr;
	
	if(vlcAddrWord) {
		HAL_DispPbFrameBufferGet(&dispAddr,&srcWidth,&srcHeight);
		//printf("src addr=%lx,w=%u,h=%u\n",dispAddr,srcWidth,srcHeight);
		pbDecodeSizeGet(&decW, &decH);
		sp1kNandRsvSizeGet(0x44, NULL, &size); /* 0320q850.hdr */
		//printf("size: %lx\n", (UINT32)size);
		
		freeBuffAddr = freeBuffAddr + (UINT32)decW * (UINT32)decH * 2UL;
		HAL_GprmDramFill(freeBuffAddr, 0x10000, 0); /* fill the 64KB dram with 0 */
		
		sp1kNandRsvRead(0x44, freeBuffAddr << 1);
		
		
		*vlcAddrWord = freeBuffAddr;
		freeBuffAddr += (size >> 1);

		srcAddr = pbDispBuffGet(1);
		dstW = SP1K_STARTUP_IMG_WIDTH;
		dstH = SP1K_STARTUP_IMG_HEIGHT;
		//printf("dec w=%u,h=%u\n",decW,decH);
		if(decW<decH)//rotate img
		{
			HAL_GprmScale(pbDispBuffGet(1),srcWidth,srcHeight,scalBuff,176,240,tempBuff,1);
			gprmBlackFill(tempBuff, PB_DECODE_WIDTH, PB_DECODE_HEIGHT);
			HAL_GprmCopy(scalBuff,176,240,0,0,tempBuff,320,240,70,0,176,240,0,1);
			srcAddr = tempBuff;
		}else if((srcWidth!=320)||(srcHeight!=240)){ //no main Image source
			if(srcWidth*9 == (srcHeight<<4))
			{
				HAL_GprmScale(srcAddr,srcWidth,srcHeight,scalBuff,320,180,tempBuff,1);
				gprmBlackFill(tempBuff, PB_DECODE_WIDTH, PB_DECODE_HEIGHT);
				HAL_GprmCopy(scalBuff,320,180,0,0,tempBuff,320,240,0,30,320,180,0,1);
				srcAddr = tempBuff;
			}
			else
			{
				HAL_GprmScale(srcAddr,srcWidth,srcHeight,scalBuff,320,240,tempBuff,1);
				srcAddr = scalBuff;
			}
		}

		jpegQTableSet(SP1K_STARTUP_IMG_QTABLE, NULL, NULL, 1);
		HAL_JpgZFactorSet( 100, 100, dstW, dstH, dstW, dstH, 0x00 );
		//dramFileSave("SETUPIMG.YUV",srcAddr,(UINT32)320*240*2);
		HAL_JpegEncode( srcAddr, freeBuffAddr, 0, 0x20, 1 );
		HAL_JpegReadEncodeSize( &vlcSize);
		
		//if (vlcSize > 64UL * 1024UL) {
			//printf("Image is too large, size %lu\n", vlcSize);
		//}

    }            
    return (size + vlcSize);
}
#endif
//xian --

//xian ++ 20080604
/**
 * @fn        UINT16 pbExifOrt_2_RotOp(UINT16 exifOrient)
 * @brief     set exif rotate option
 * @param     [in] exifOrient : 1->0, 6->90, 3->180, 8->270
 * @retval    return = 0->0, 1->90, 2->180, 3->270
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-6
 * @todo      N/A
 * @bug       N/A
*/
#if (!STILL_LIB_PB_DMY)
static UINT16 pbExifOrt_2_RotOp(UINT16 exifOrient)
{
    UINT8 rotOp;    
    
    switch(exifOrient){
        case 1:
            rotOp = (UINT16)PB_ROT_0;
            break;            
        case 6:
            rotOp = (UINT16)PB_ROT_90;
            break;            
        case 3:
            rotOp = (UINT16)PB_ROT_180;
            break;            
        case 8:
            rotOp = (UINT16)PB_ROT_270;
            break;
        default:
            rotOp = (UINT16)PB_ROT_0;
            break;                    
    }

    return rotOp;
}

/**
 * @fn        UINT8 pbRotModeGet(void)
 * @brief     modified from pbDecRotModeGet(void).select a proper rotation value 		  
 * @param     UINT8 option
 							option =0; GPRM rotation paramter
 							option =1; JPG rotation paramter
 							option =2; Exif rotation parameter
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    hy.ren
 * @since     2011-03-29
 * @todo      N/A
 * @bug       N/A
*/
UINT8 pbRotModeGet(UINT8 option)
{
	UINT16 rotOp;
	UINT8 rotTab[3][4] = {
			{0,5,3,6}, //GPRM Rotation flag
			{0,5,6,3}, //JPG Rotation flag
			{1,6,3,8}  //Exif Rotation flag
	};
	
	rotOp = pbExifOrt_2_RotOp(pbParam[PB_PARAM_EXIF_ORIENT]);
	pbLogPrintf("exifOrt = %u -> rotOp = %u\n", pbParam[PB_PARAM_EXIF_ORIENT], rotOp);

	rotOp = rotOp + pbParam[PB_PARAM_ROT];
	if(rotOp >= PB_ROT_SET_NUM){
		rotOp = rotOp - (UINT8)PB_ROT_SET_NUM;
	}

	return rotTab[option][rotOp];

}

UINT8 pbCropRotModeGet(PBGPRMRotParam_t rotMode)
{
	switch( rotMode ){
		case PB_GPRM_ROT_0:
			rotMode = PB_GPRM_ROT_0;
		break;            
		case PB_GPRM_ROT_90:
			rotMode = PB_GPRM_ROT_270;
			break;            
		case PB_GPRM_ROT_180:
			rotMode = PB_GPRM_ROT_180;	
		break;            
		case PB_GPRM_ROT_270:		
			rotMode = PB_GPRM_ROT_90;	
		break;            
		default:
			break;
	}

	return rotMode;
}

/**
 * @fn        UINT16 pbExifOrientGet(void)
 * @brief     match exif rotation option with orient option
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-6
 * @todo      N/A
 * @bug       N/A
*/
UINT16 pbExifOrientGet(void)
{
	return 0;
#if 0 //not use 
    UINT16 rotOp;
    UINT16 exifOrient = 1;
    
    rotOp = pbExifOrt_2_RotOp(pbParam[PB_PARAM_EXIF_ORIENT]);
#if (DBG_Level > 1)	
    DEBUG_PRINT("exifOrt = %u -> rotOp = %u\n", pbParam[PB_PARAM_EXIF_ORIENT], rotOp);
#endif 
    rotOp = rotOp + pbParam[PB_PARAM_ROT];	
#if (DBG_Level > 1)
    DEBUG_PRINT("rotOp = %u\n", rotOp);
#endif 
    if(rotOp >= PB_ROT_SET_NUM)
        rotOp = rotOp - (UINT8)PB_ROT_SET_NUM;
        
    switch(rotOp){
        case PB_ROT_0:
            exifOrient = 1;
            break;            
        case PB_ROT_90:
            exifOrient = 6;
            break;            
        case PB_ROT_180:
            exifOrient = 3;
            break;            
        case PB_ROT_270:
            exifOrient = 8;
            break;            
        default:
            break;
    }
#if (DBG_Level > 1)    
    DEBUG_PRINT("rotOp = %u -> exifOrt = %u\n", rotOp, exifOrient);
#endif //(DBG_Level > 1)
	
	//printf("exifOrient=%x\n",exifOrient);
    return exifOrient;
#endif
}
#endif
/**
 * @fn        UINT8 pbImgRotate(PBFile_t* pFile)
 * @brief     modify rotate flag and save file
 * @param     [in] pFile
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-6
 * @todo      N/A
 * @bug       N/A
*/
#if (STILL_LIB_PB_DMY)
UINT8 pbImgRotate(PBFile_t* pFile)
{
        pFile=pFile;
        return SUCCESS;
}
#else
UINT8 pbImgRotate(PBFile_t* pFile)
{

	UINT8 sts, ret = SUCCESS;
	UINT16 fHdl;
	UINT32 fileSize;
	UINT32 dramEndAddr = K_SDRAM_TotalSize - 256UL;//512 Bytes reserved for DMA
	UINT32 freeBuffAddr = K_SDRAM_ImagePlaybackBufAddr;
	UINT32 srcAddr;
     UINT8 exifRot;
	 
	ExifImage_t mainImage;
	//PbCropImage_t image;
	
	fHdl = vfsFileOpen(pFile->FileName, K_DOS_ReadFile, pFile->FileStartCluster);
	if (!fHdl) {
		DBG_pbFuncTrack();
		return FAIL;
	}

	fileSize = vfsFileSizeGet(fHdl);
	srcAddr = dramEndAddr - (fileSize >> 1);
	sts = vfsFileRead(fHdl, K_DOS_DstDram, fileSize, srcAddr, 0);

	if (sts != TRUE) {
		vfsFileClose(fHdl, pFile->FileName, K_DOS_ReadFile, 0);
		DBG_pbFuncTrack();
		return FAIL;
	}
    
	exifFillRotOpt = 0;
	
	sts = exifParserStart(exifGetMainInfo,&mainImage,srcAddr, fileSize);
	
	vfsFileClose(fHdl,  pFile->FileName, K_DOS_ReadFile, 0);
	if (sts != TRUE) {
		return FAIL;
	}
	// add by rhy 20100329
	if(pbParam[PB_PARAM_JPG_ROT]){
		Rotation_En =1;   //enable JPG rotation
	}
	#if 0 //real rotation funciton is not use ,so masked
	if(Rotation_En){
		image.srcWidth = mainImage.width;
		image.srcHeight = mainImage.height;
		pbEdit_ImgDec(pFile,PB_ROT,&image);
		ret = pbEdit_Save(PB_ROT,image);
	}
	#endif
	//else{
#if (DBG_Level > 1)
	DEBUG_PRINT("org exif ort: %ld\n", exifRot);
#endif 

	exifRot = (UINT32)pbRotModeGet(2);
#if (DBG_Level > 1) 
	DEBUG_PRINT("modify exif ort: %ld\n", exifRot);
#endif 
	exifFillRotOpt = 1;
	sts = exifParserStart(exifGetMainInfo,&mainImage,srcAddr, fileSize);
	exifSetIFDData(EXIF_IFD_0,EXIF_TAG_ORIENTATION,1,exif_swap_ushort((UINT16)exifRot));
	
	ret = pbEditFileMake(srcAddr, fileSize);
	exifFillRotOpt = 0;
	//}   
	return ret;
}
#endif
//xian --

//xian ++ 20080616
//return file addr
/**
 * @fn        UINT32 pbEditFileLoad(PBFile_t* pFile, Exif_Decompress_Parameter* mainPara, Exif_Decompress_Parameter* thumbPara, Exif_Decompress_Parameter* qvPara)
 * @brief     read and parse file
 * @param     [in] pFile
 * @param     [in] mainPara
 * @param     [in] thumbPara
 * @param     [in] qvPara
 * @retval    return = file address
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-6
 * @todo      N/A
 * @bug       N/A
*/
#if (!STILL_LIB_PB_DMY)
UINT32 pbEditFileLoad(
	PBFile_t* pFile,
	ExifImage_t* mainPara, 
	ExifImage_t* thumbPara,
	ExifImage_t* qvPara
)
{
	UINT8 sts;
	UINT32 dramEndAddr = K_SDRAM_TotalSize - 256UL;//512 Bytes reserved for DMA
	UINT32 freeBuffAddr = K_SDRAM_ImagePlaybackBufAddr;
	UINT32 fileSize, fileAddr;
	UINT16 fHdl;

	OrderToAddr(freeBuffAddr, 16);
    
	fHdl = vfsFileOpen(pFile->FileName, K_DOS_ReadFile, pFile->FileStartCluster);
	if (!fHdl) {
		DBG_pbFuncTrack();
		return FAIL;
	}

	fileSize = vfsFileSizeGet(fHdl);
	fileAddr = dramEndAddr - (fileSize >> 1);
	sts = vfsFileRead(fHdl, K_DOS_DstDram, fileSize, fileAddr, 0);

	if (sts != TRUE) {
		vfsFileClose(fHdl, pFile->FileName, K_DOS_ReadFile, 0);
		DBG_pbFuncTrack();
		return FAIL;
	}
	sts = exifParserStart(exifGetThumbInfo,thumbPara,fileAddr, fileSize);// bug here 
	sts = exifParserStart(exifGetMainInfo,mainPara,fileAddr, fileSize);
	sts = exifParserStart(exifGetQVInfo,qvPara,fileAddr, fileSize);
	
	vfsFileClose(fHdl,  pFile->FileName, K_DOS_ReadFile, 0);
	if (sts != TRUE) {
		DBG_pbFuncTrack();
		return FAIL;
	}
#if 0
	if (qvPara && exifAppN) {
		qvPara->addr = exifQvVlcAddr;
		qvPara->dataType = mainPara->dataType;
		qvPara->width = exifQvXSize;
		qvPara->height = exifQvYSize;
	}
#endif
	//printf("fileAddr = 0x%lx\n", fileAddr);
	return fileAddr;
}

//return SUCCESS/FAIL
/**
 * @fn        UINT8 pbEditScaleFacCal(UINT32 bufSize, UINT16 srcW, UINT16 srcH, UINT16 dstW)
 * @brief     get edit scale factor
 * @param     [in] bufSize
 * @param     [in] srcW
 * @param     [in] srcH
 * @param     [in] dstW
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-6
 * @todo      N/A
 * @bug       N/A
*/
static UINT8 pbEditScaleFacCal(
	UINT32 bufSize,
	UINT16 srcW,
	UINT16 srcH,
	UINT16 dstW
)
{
	UINT32 tmpFac;
	UINT8 fac;

	//scale fac: w*h*(fac/8)*(fac/8)
	tmpFac = (bufSize << 6) / ((UINT32)srcW * (UINT32)srcH);
#if (DBG_Level > 1) 	
	DEBUG_PRINT("tmpFac = %ld\n", tmpFac);
#endif //(DBG_Level > 1)
        
	fac = 0x08;
	while(fac) {
		if((UINT32)fac * (UINT32)fac > tmpFac) {
			//fac--;	/*if jpg engine can set roi, fac can be odd then. */
			fac-=2;		/*width should be multiple of 16*/
		} else {
			break;
		}
	}

	if (srcW > dstW) {
		//while (((srcW >> 3) * --fac) > dstW);
		while (((srcW >> 3) * fac) > dstW)  //modified by rhy,20100328
			fac--;
		if(fac == 0) {
			fac = 1;
		}// decopress width < decWidth
	} else if ( tmpFac > 64) {
		fac = 0;
	}

	if (fac) {
        if (((UINT32)srcW * (UINT32)srcH * (UINT32)fac * (UINT32)fac)
            > (bufSize << 6)){
			DBG_pbFuncTrack();
			return 0xff;
        }        
	} else {
        if (((UINT32)srcW * (UINT32)srcH) > (bufSize << 3)){
			DBG_pbFuncTrack();
			return 0xff;
        }        
	}
#if (DBG_Level > 1) 		
	DEBUG_PRINT("fac %bu\n", fac); 
#endif //(DBG_Level > 1)   
	return fac;
}

/**
 * @fn        UINT8 pbEditDec(Exif_Decompress_Parameter* imagePara, UINT32 decAddr, UINT32 endAddr, UINT16 *dstWidth, UINT16 *dstHeight)
 * @brief     decode image
 * @param     [in] imagePara
 * @param     [in] decAddr
 * @param     [in] endAddr
 * @param     [in] dstWidth
 * @param     [in] dstHeight
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-6
 * @todo      N/A
 * @bug       N/A
*/
static UINT8 pbEditDec(
	ExifImage_t* imagePara,
	UINT32 decAddr,
	UINT32 fileAddr,
	UINT32 endAddr,
	UINT16 *dstWidth,   //input/output
	UINT16 *dstHeight   //input/output
)
{
	UINT8 opt;
	UINT32 srcAddr, tmpDecAddr, tmpCentAddr, tmpScaleAddr;
	UINT16 srcW, srcH;
	UINT16 dstW, dstH;
	UINT16 decW, decH, dmyW;
	UINT16 tmpBlkH, tmpOffH; //when buffer is not enough to move center
	UINT8 fac;

	if (imagePara->dataType == K_File_DataType_YUV420) {
		opt = 0x05;
	} else if (imagePara->dataType == K_File_DataType_YUV422) {
		opt = 0x04;
	} else {
		printf("YUV err\n");
		return FAIL;
	}

	srcAddr = imagePara->addr;
	srcW = imagePara->width;
	srcH = imagePara->height;
	dstW = *dstWidth;
	dstH = *dstHeight;

	if ((srcW & 0x000f) != 0x00) {
		OrderToAddr(srcW, 16);
	}

	fac = pbEditScaleFacCal(endAddr - decAddr, srcW, srcH, dstW);
	if(fac == 0xff)
		return FAIL;

	dmyW = 0;
	if(fac){
		decW = (((srcW * (UINT16)fac) >> 3));
		decH = (((srcH * (UINT16)fac) >> 3));
		dmyW = decW & 0x7;

		if(dmyW){
			dmyW = 8 - dmyW;
			decW += dmyW;
		}

		if(((decW > dstW || decH > dstH) || (decAddr + (UINT32)decW *decH) >= fileAddr)&& fac > 1){
			fac --;
			decW = (((srcW * (UINT16)fac) >> 3));
			decH = (((srcH * (UINT16)fac) >> 3));
			dmyW = decW & 0x7;

			if(dmyW){
				dmyW = 8 - dmyW;
				decW += dmyW;
			}
		}
	}
	else{
		decW = srcW;
		decH = srcH;
	}  

	//alloc addr : tmpDecAddr -> tmpCentAddr -> decAddr
	if(decW > dstW || decH > dstH){// 3 buffers
		tmpDecAddr = decAddr;
		tmpCentAddr = decAddr + (UINT32)decW * (UINT32)decH;
		tmpScaleAddr = tmpCentAddr + (UINT32)decW * (UINT32)decH;
		if((tmpScaleAddr + (UINT32)decW * (UINT32)dstH) > endAddr) {
			DBG_pbFuncTrack();
			return FAIL;
		} 
	            
		if(dmyW){// move to center, scale down
			tmpBlkH = decH;
		}
		else{// scale down only
			tmpDecAddr = tmpCentAddr;
		}
	}
	else if(dmyW){// 2 buffers        
		tmpDecAddr = decAddr + (UINT32)decW * (UINT32)decH;
		tmpCentAddr = decAddr;
		tmpScaleAddr = 0;
		if((tmpDecAddr + (UINT32)decW * (UINT32)decH) > endAddr) {
			tmpBlkH = ((endAddr - decAddr)/(UINT32)decW - decH) & ~7;
			#if (DBG_Level > 1) 				
				DEBUG_PRINT("tmpBlkH=%u\n", tmpBlkH); 
			#endif //(DBG_Level > 1)  
			tmpDecAddr = decAddr + (UINT32)decW * (UINT32)tmpBlkH;

			if(!tmpBlkH){
				DBG_pbFuncTrack();
				return FAIL;
			}
		}
		else{
			tmpBlkH = decH;
		}

	}
	else{// 1 buffer
		tmpDecAddr = decAddr;
		tmpCentAddr = 0; 
		tmpScaleAddr = 0;        
	}
#if (DBG_Level > 1)          
	//DEBUG_PRINT("size : %u, %u, %u, %u, %u\n", srcW, srcH, decW, decH, dmyW);
    //printf("addr : 0x%lx -> 0x%lx (0x%lx, 0x%lx, 0x%lx)\n", srcAddr, decAddr, tmpDecAddr, tmpCentAddr, tmpScaleAddr);
#endif //(DBG_Level > 1)  

	//printf("size : %d, %d, %d, %d, %bd\n", srcW, srcH, decW, decH, fac);
	//printf("vlc src addr=0x%lx\n",srcAddr);
	//printf("vlc yuv addr=0x%lx\n",tmpDecAddr);
	//printf("opt=%bd\n",opt);

	if( exifVlcAtOdd == 0 ) {
		HAL_JpegDecode(srcAddr, srcW, srcH, tmpDecAddr, fac, opt, G_JpgRstMcuNum);
	}
	else {
		HAL_JpegDecode(srcAddr, srcW, srcH, tmpDecAddr, fac, ( opt | 0x10 ), G_JpgRstMcuNum );
	}

	//dramFileSave("PBMAIN00.YUV", decAddr, (UINT32)srcW*srcH*2);
	
	if(dmyW){//move to center
		tmpOffH = 0;
		while(tmpOffH < decH){
			if(decH - tmpOffH < tmpBlkH){
				tmpBlkH = decH - tmpOffH;
			}

			gprmBlackFill(tmpCentAddr + (UINT32)tmpOffH * (UINT32)decW, decW, tmpBlkH);   //fill dst buffer
			HAL_GprmCopy(
				tmpDecAddr + (UINT32)tmpOffH * (UINT32)decW,
				decW, tmpBlkH, 0, 0,
				tmpCentAddr + (UINT32)tmpOffH * (UINT32)decW,
				decW, tmpBlkH, ((dmyW >> 1) & ~0x01), 0,
				decW - dmyW, tmpBlkH,
				0,
				1
			);

			tmpOffH += tmpBlkH;
		}
	}
//printf("decW=%u,decH=%u,dstW=%u,dstH=%u\n",decW,decH,dstW,dstH);
	if(tmpScaleAddr){	/*scale down*/
		HAL_GprmScale(
			tmpCentAddr,
			decW, decH,
			decAddr,
			dstW, dstH,
			tmpScaleAddr,
			1
		);
		decW = dstW;
		decH = dstH;
	}
	    
	*dstWidth = decW;
	*dstHeight = decH;

	//dramFileSave("PBDOWN00.YUV", decAddr, (UINT32)dstW*dstH*2);
	
	#if (DBG_Level > 1)        
		DEBUG_PRINT("decW=%u, decH=%u\n", decW, decH);   
	#endif 
	
	return SUCCESS;
}

//return addr
UINT32 pbEditImgMoveBufEnd(UINT32 srcAddr, UINT16 imgW, UINT32 imgH, UINT32 endBufAddr)
{
    UINT32 dstAddr = srcAddr;
    UINT32 blkBufSize;
    UINT16 blkH, tolH;

#if (DBG_Level > 1)   
    printf("move end from : 0x%lx\n", srcAddr);
#endif //(DBG_Level > 1)  
    srcAddr += (UINT32)imgW * (UINT32)imgH; //end addr
    if((srcAddr + imgW*2) >= endBufAddr) //  at least 2 lines buffer
        return dstAddr; //not move
        
    blkBufSize = ((endBufAddr - srcAddr - 16) & ~15);    
    if(blkBufSize/(UINT32)imgW > (UINT32)imgH)
        blkH = imgH;
    else
        blkH = (UINT16)(blkBufSize/(UINT32)imgW);
 #if (DBG_Level > 1)    
    DEBUG_PRINT("blkH=%u\n", blkH);
 #endif //(DBG_Level > 1)     
    if(blkH){
        dstAddr = endBufAddr & ~15;

        tolH = 0;
        while(tolH < imgH){
            if(imgH - tolH < blkH)
                blkH = imgH - tolH;
            
            tolH += blkH;
            
    		HAL_GprmCopy(
    			srcAddr - (UINT32)imgW * (UINT32)tolH,
    			imgW, blkH, 0, 0,
    			dstAddr - (UINT32)imgW * (UINT32)tolH,
    			imgW, blkH, 0, 0,
    			imgW, blkH,
    			0,
    			1
    		);
        }
        dstAddr = dstAddr - (UINT32)imgW * (UINT32)tolH;        
    }
    else{
        dstAddr = srcAddr;
    }
#if (DBG_Level > 1)     
    DEBUG_PRINT("           to : 0x%lx\n", dstAddr);
#endif //(DBG_Level > 1)  
    return dstAddr;
}

UINT8 pbEdit_ImgDec(PBFile_t* pFile,pbEditType_t editType,PbCropImage_t *cropRec)
{
	ExifImage_t mainImge;

	UINT8 ret = SUCCESS;
	UINT32 dramEndAddr = K_SDRAM_TotalSize - 256UL;//512 Bytes reserved for DMA
	UINT32 freeBuffAddr = K_SDRAM_ImagePlaybackBufAddr;
	UINT32  fileAddr, decAddr;
	UINT32 hdrSize = (PB_EDIT_IMG_HDR_SIZE * 2UL);
	UINT32 yuvBufInterval;
	UINT16 decW, decH;
	UINT8 rotMode;
	UINT32 rotBufAddr;
	UINT16 temp;
	
	OrderToAddr(freeBuffAddr, 16);
	if(editType != PB_CROP){
	    fileAddr = pbEditFileLoad(pFile, &mainImge, NULL, NULL);
	    if(!fileAddr){
	        return FAIL;
	    }
	}
	else{
		mainImge.width = pFile->MediaWidth;
		mainImge.height = pFile->MediaHeight;
	}
	yuvBufInterval = (UINT32)PB_EDIT_CDSP_DO_LINE_NUM * ((mainImge.width>1024)?(UINT32)mainImge.width:1024UL); // reserved for overlay
	if (yuvBufInterval> ((UINT32)PB_DECODE_QV_WIDTH*PB_DECODE_QV_HEIGHT*2 + (UINT32)PB_DECODE_THB_WIDTH*128)	){
		decAddr=  freeBuffAddr + yuvBufInterval+ (hdrSize>>1);
	}
	else{
		decAddr=  freeBuffAddr + (hdrSize>>1) + (UINT32)PB_DECODE_QV_WIDTH*PB_DECODE_QV_HEIGHT + (UINT32)PB_DECODE_THB_WIDTH*128;//real rotation thumbnail height is 128
	}

	OrderToAddr(decAddr, 16);
	//printf("freeBuffAddr=0x%lx,decAddr=0x%lx\n",freeBuffAddr,decAddr);
	
	switch(editType){
		case PB_ROT:
			#if 0 //real rotation funciton is not use ,so masked
			if(Rotation_En){
				decW = MCU_ALIGN(mainImge.width, 16);
				decH = MCU_ALIGN(mainImge.height, 16);
				cropRec->recWidth = MCU_ALIGN(cropRec->srcWidth,16);
				cropRec->recHeight = MCU_ALIGN(cropRec->srcHeight,16);   //keep MCU Align;
			}
			#endif
		break;
		case PB_RESIZE:
			cropRec->recWidth = cropRec->srcWidth;
			cropRec->recHeight = cropRec->srcHeight;
			decW = cropRec->recWidth;
			decH = cropRec->recHeight;
		break;
		case PB_IMG_EFFECT: //effect ...
			decW = mainImge.width;
			decH = mainImge.height;
			cropRec->recWidth = mainImge.width;
			cropRec->recHeight = mainImge.height;
		break;
		case PB_CROP:
			
			cropRec->recWidth =  MCU_ALIGN(cropRec->recWidth,8); //keep corp region aligment 
			cropRec->recHeight =  MCU_ALIGN(cropRec->recHeight,8);
			freeBuffAddr = decAddr + (UINT32)cropRec->srcWidth*cropRec->srcHeight;
			
			/* get GPRM rotation parameter  */	
			rotMode = pbCropRotModeGet(pbDecRotModeGet());
			if(rotMode & 0x4){
				cropRec->startX = MCU_ALIGN(cropRec->startX,2);
				cropRec->startY = MCU_ALIGN(cropRec->startY,2);
				rotBufAddr = freeBuffAddr + (UINT32)cropRec->srcWidth*cropRec->srcHeight;
				if(rotBufAddr > K_SDRAM_TotalSize){
					return FAIL;
				}
				//printf("crop w=%u,h=%u,start X=%u,y=%u\n",cropRec->recWidth,
				//cropRec->recHeight,cropRec->startX,cropRec->startY);
				#if 0
				HAL_GprmRotExt(
					rotMode, 
					cropRec->srcAddrW, 
					freeBuffAddr,
					cropRec->srcWidth, cropRec->srcHeight,
					cropRec->startX, cropRec->startY,
					cropRec->recWidth, cropRec->recHeight,
					1
				);	
				#else 
				HAL_GprmCopy(
					cropRec->srcAddrW,
					cropRec->srcWidth, cropRec->srcHeight,cropRec->startX, cropRec->startY,
					rotBufAddr, //temp buffer ...
					cropRec->recWidth, cropRec->recHeight, 0, 0,  
					cropRec->recWidth, cropRec->recHeight,
					0,
					1
				);

				HAL_GprmRot(
					rotMode, 
					rotBufAddr, 
					freeBuffAddr,
					cropRec->recWidth, cropRec->recHeight,
					1
				);
				#endif
				
				temp = cropRec->recWidth;
				cropRec->recWidth = cropRec->recHeight;
				cropRec->recHeight = temp;
			}
			else{ 
				HAL_GprmCopy(
					cropRec->srcAddrW,
					cropRec->srcWidth, cropRec->srcHeight,cropRec->startX, cropRec->startY,
					freeBuffAddr,
					cropRec->recWidth, cropRec->recHeight, 0, 0,  
					cropRec->recWidth, cropRec->recHeight,
					0,
					1
				);
			}
		
			HAL_GprmCopy(
				freeBuffAddr,
				cropRec->recWidth, cropRec->recHeight, 0, 0,  
				decAddr,
				cropRec->recWidth, cropRec->recHeight, 0, 0,  
				cropRec->recWidth, cropRec->recHeight,
				0,
				1
			);
			//printf("rec w=%u,h=%u\n",cropRec->recWidth,cropRec->recHeight);
			cropRec->srcAddrW = decAddr;
			cropRec->srcWidth = cropRec->recWidth;
			cropRec->srcHeight = cropRec->recHeight;
			cropRec->recWidth= mainImge.width;
			cropRec->recHeight= mainImge.height;
			
			goto quick_exit;
		break;
	}
	
	/* decode Main YUV	 */ 			
	ret = pbEditDec(&mainImge, decAddr, fileAddr,dramEndAddr, &decW, &decH);
	if(ret){
		return FAIL;
	}
	
	cropRec->srcAddrW = decAddr; 
	cropRec->srcWidth = decW;
	cropRec->srcHeight = decH;
quick_exit:

	//dramFileSave("OOOOVVVV.YUV", decAddr, (UINT32)cropRec->srcWidth*cropRec->srcHeight*2);
	return ret;
}

UINT8 pbEdit_Save(pbEditType_t imgEffect,PbCropImage_t cropRec)
{
	UINT8 ret = FAIL;
	UINT32 dramEndAddr = K_SDRAM_TotalSize - 256UL;//512 Bytes reserved for DMA
	UINT32 freeBuffAddr = K_SDRAM_ImagePlaybackBufAddr;
	UINT32 hdrAddr;
	UINT32 effetAddr1;
	UINT32 hdrSize = PB_EDIT_IMG_HDR_SIZE * 2UL;
	UINT16 hfac = 100,vfac = 100;
	UINT32 effAddr;
	UINT32 tmbEffAddr;//qvEffAddr; 
	UINT32 sclTmpAddr;
	UINT32 vlcAddr;
	UINT32 vlcSize;
	UINT32 mainVlcAddr, tmbVlcSize, mainVlcSize;
	UINT32 size;   
	UINT8 sizeFlag;
	UINT8 yuvType = 0x20;
    UINT8 option = 0; //default value
	UINT8 rotation = 0;
	UINT16 RotThumbHeight = PB_DECODE_THB_HEIGHT;
	UINT32 dstWidth;
	UINT32 dstHeight;
	UINT16 qvWidth=PB_DECODE_QV_WIDTH;
	UINT16 qvHeight =PB_DECODE_QV_HEIGHT;
	UINT16 thbWidth=PB_DECODE_THB_WIDTH;
	UINT16 thbHeight =PB_DECODE_THB_HEIGHT; //default value ...
	exifJpgFileInfo_t jpgExifInfo;
	
	OrderToAddr(freeBuffAddr, 16);
    pbLogPrintf("buf: 0x%lx\n", freeBuffAddr);

	//printf("srcw=%u,srch=%u,recw=%u,rech=%u\n",cropRec.srcWidth,cropRec.srcHeight,cropRec.recWidth,cropRec.recHeight);
	/*real rotation image ...*/
	if((cropRec.srcWidth < cropRec.srcHeight) && ((pbRotModeGet(0)!= 0x6) ||(pbRotModeGet(0)!= 0x8)) ){ //real rotation image...
		dstWidth = MCU_ALIGN(cropRec.recHeight,16);
		dstHeight = MCU_ALIGN( cropRec.recWidth,16);
		
		qvWidth = PB_DECODE_QV_HEIGHT;
		qvHeight = PB_DECODE_QV_WIDTH;
		thbHeight = PB_DECODE_THB_WIDTH;
		RotThumbHeight = PB_DECODE_THB_WIDTH;
		thbWidth = 128;
	}
	else{
		dstWidth = cropRec.recWidth;
		dstHeight = cropRec.recHeight;
	}
	#if 0 //real rotation funciton is not use ,so masked
	if(Rotation_En){ //add by rhy 20100328,add the function of really rotation.
		rotation = pbRotModeGet(1);
		yuvType |= (rotation<<2);
		option = rotation;
	}
	#endif
	exifVlcAtOdd = 0;
	
	/* Do QV YUV */ 	
	effAddr = freeBuffAddr + (hdrSize>>1); //reserved 64Kb buffer 
	OrderToAddr(effAddr,16);
	effetAddr1 = effAddr;

	if(cropRec.recWidth*3 != 4*cropRec.recHeight){ //
		gprmBlackFill(effAddr,qvWidth, qvHeight);
		effAddr += qvWidth*30;
		qvHeight = 180;
	}
	/*attention,if srcwidth and srcheight < dstwidth and dstheight,can't use pbCdspEffectDo*/
	if( imgEffect&0x10 ){
		//qvEffAddr = effAddr + (UINT32)qvWidth * qvHeight;
		sclTmpAddr = cropRec.srcAddrW +  (UINT32)cropRec.srcWidth*cropRec.srcHeight;
		HAL_GprmScale(
			cropRec.srcAddrW,
			cropRec.srcWidth, cropRec.srcHeight,
			effAddr,
			qvWidth, qvHeight,
			sclTmpAddr,
			1
		);
	}			
	else{
		pbCdspEffectDo(imgEffect , cropRec.srcAddrW, cropRec.srcWidth, cropRec.srcHeight, effAddr, qvWidth, qvHeight); 
	}
	qvHeight = 240;
	effAddr = effetAddr1; 
	
	//dramFileSave("QQQQVVVV.YUV", effAddr, (UINT32)qvWidth*qvHeight*2);
	/* Do thumbnail YUV	 */ 
	tmbEffAddr = effAddr + (UINT32)qvWidth * qvHeight;
	#if 0  //not use 
	/* GPRM scale down from qv to thumbnail */
	sclTmpAddr = tmbEffAddr + (UINT32)thbWidth* thbHeight;
	if(sclTmpAddr + (UINT32)thbWidth* thbHeight> dramEndAddr){
		DBG_pbFuncTrack();
		return FAIL;
	}
	#endif
	#if 0//real rotation funciton is not use ,so masked
	if(Rotation_En ){
		if(rotation&0x1){
			RotThumbHeight = 128;
		}
	}
	#endif
	HAL_GprmScale(
		effAddr,
		qvWidth, qvHeight,
		tmbEffAddr,
		thbWidth, RotThumbHeight,
		freeBuffAddr,
		1
	);
	pbLogPrintf("thb addr= 0x%lx\n", tmbEffAddr);
	//dramFileSave("PBTHM001.YUV", tmbEffAddr, (UINT32)PB_DECODE_THB_WIDTH*RotThumbHeight*2);
	jpgExifInfo.qvQidx = 85;
	
	do{	
		sizeFlag = FALSE; //init falg;
		vlcAddr = freeBuffAddr;
		jpegQTableSet(jpgExifInfo.qvQidx, NULL, NULL, 1); 
		HAL_JpgZFactorSet( hfac, vfac, qvWidth, qvHeight, qvWidth, qvHeight, option);

		HAL_JpegEncode( effAddr, vlcAddr, 0, yuvType/*0x20*/, 1 );
		HAL_JpegReadEncodeSize( &vlcSize);
		
		jpgExifInfo.qvVlcAddrW = vlcAddr;
		jpgExifInfo.qvSize = vlcSize;
		jpgExifInfo.qvWidth = qvWidth;
		jpgExifInfo.qvHeight = qvHeight;
		#if 0//real rotation funciton is not use ,so masked
		if(Rotation_En ){
			if(rotation&0x1 ){
				exifQvXSize = PB_DECODE_QV_HEIGHT;
				exifQvYSize = PB_DECODE_QV_WIDTH;
			}
		}
		#endif
		pbLogPrintf("qv vlc: 0x%lx, 0x%lx\n", vlcAddr, vlcSize);
	 	
		/* generate Tmb VLC  */ 	  
		vlcAddr = vlcAddr + ((vlcSize + 1)>> 1) ;

		jpgExifInfo.thbQidx = 85;
		jpegQTableSet(jpgExifInfo.thbQidx, NULL, NULL, 1);
		#if 0//real rotation funciton is not use ,so masked
		if(Rotation_En ){		
			HAL_JpgZFactorSet( hfac, vfac, thbWidth, RotThumbHeight, thbWidth, RotThumbHeight, option );
		}
		else{
			HAL_JpgZFactorSet( hfac, vfac, thbWidth, thbHeight, thbWidth, thbHeight, option );
		}
		#endif
		HAL_JpgZFactorSet( hfac, vfac, thbWidth, thbHeight, thbWidth, thbHeight, option );
		HAL_JpegEncode( tmbEffAddr, vlcAddr, 0, yuvType/*0x20*/, 1 );
		HAL_JpegReadEncodeSize( &vlcSize );
		
		//dramFileSave("PBTHM001.YUV", tmbEffAddr, (UINT32)PB_DECODE_THB_WIDTH*RotThumbHeight*2);
		pbLogPrintf("Tmb vlc: 0x%lx, 0x%lx\n", vlcAddr, vlcSize);
		jpgExifInfo.thbVlcAddr =  vlcAddr;
		jpgExifInfo.thbSize = vlcSize;
		jpgExifInfo.thbWidth = thbWidth;
		jpgExifInfo.thbHeight =  RotThumbHeight;
		#if 0 //real rotation funciton is not use ,so masked
		if(Rotation_En){
			if(rotation&0x1){
				exifThbXSize = RotThumbHeight;
				exifThbYSize = PB_DECODE_THB_WIDTH;
			}
		}
		#endif
		tmbVlcSize = vlcSize;
		if( tmbVlcSize & 0x00000001 ) {
			size = ( K_EXIF_STRUCTURE0_SIZE + tmbVlcSize + K_EXIF_STRUCTURE3_SIZE + K_EXIF_STRUCTURE4_SIZE );
		}
		else {
			size = ( K_EXIF_STRUCTURE0_SIZE + tmbVlcSize + K_EXIF_STRUCTURE2_SIZE + K_EXIF_STRUCTURE4_SIZE );
		}
		if( 65536 - size< ( (UINT32)K_HDR2_SIZE + jpgExifInfo.qvSize) ) {
			pbLogPrintf( "qv size too large\n" );
			sizeFlag = TRUE;
			jpgExifInfo.qvQidx -=10;
		}
	
	}while(sizeFlag);
 	pbLogPrintf("thm vlc addr=0x%lx,size=0x%lx\n",vlcAddr,tmbVlcSize);
 
	/* Do Main YUV		 */ 		
	effAddr = freeBuffAddr + (hdrSize >> 1);
	OrderToAddr(effAddr,16);

	if( imgEffect&0x10 ){
		effAddr = cropRec.srcAddrW;
	}			
	else{
		pbCdspEffectDo(imgEffect, cropRec.srcAddrW, cropRec.srcWidth, cropRec.srcHeight, effAddr, cropRec.srcWidth, cropRec.srcHeight); 
	}		
	
	pbLogPrintf("effer src=0x%lx,dst=0x%lx\n",cropRec.srcAddrW,effAddr);
	pbLogPrintf("w=%u,h=%u\n",cropRec.srcWidth,cropRec.srcHeight);
	/*move to end buf, xian ++ 20080626*/
	effAddr = pbEditImgMoveBufEnd(effAddr, cropRec.srcWidth, cropRec.srcHeight, dramEndAddr);
	
	/*encode to jpeg*/
	mainVlcAddr = freeBuffAddr + (hdrSize >> 1);
	jpegQTableSet(PB_EDIT_IMG_QTAB, NULL, NULL, 1);
	HAL_JpgZFactorSet( hfac, vfac,cropRec.srcWidth, cropRec.srcHeight, dstWidth, dstHeight, option );
	HAL_JpegEncode( effAddr, mainVlcAddr, 0, yuvType, 1 );
	HAL_JpegReadEncodeSize( &mainVlcSize);
	pbLogPrintf("main Vlc addr=0x%lx,main vlc size=0x%lx\n",mainVlcAddr,mainVlcSize);	

	#if 0 //real rotation funciton is not use ,so masked
	if(Rotation_En  ){
		if(rotation&0x01 ){
			jpgExifInfo.mainWidth = dstHeight;
			jpgExifInfo.mainHeight = dstWidth;
		}
	}
	#endif
    /*make file*/
	hdrAddr = dramEndAddr - PB_EDIT_IMG_HDR_SIZE;
    if(hdrAddr > mainVlcAddr + mainVlcSize){	
		#if 0 //real rotation funciton is not use ,so masked
		if(Rotation_En ){
			exifRotOpt = 0x01;
		}
		else{
			exifRotOpt = (UINT32)pbExifOrientGet();
		}
		#endif
	    exifFillRotOpt = 1;
		jpgExifInfo.mainVlcAddrW = mainVlcAddr;
		jpgExifInfo.mainWidth = dstWidth;
		jpgExifInfo.mainHeight = dstHeight;
		jpgExifInfo.mainQidx = PB_EDIT_IMG_QTAB;
		jpgExifInfo.fileFormat = 0x00;
		jpgExifInfo.rotOpt = pbRotModeGet(2);
		jpgExifInfo.viViLinkEn = 0;
		jpgExifInfo.dbgInfoAddr = 0;

#if 0 // gps tag test
{
extern	UINT32 exifGPSTagTest(const exifJpgFileInfo_t *jpgInfo);
		hdrSize = exifGPSTagTest(&jpgExifInfo);		
}
#else
		hdrSize = exifCreateFile(&jpgExifInfo);
#endif
		
		ret = pbEditFileMake(mainVlcAddr - (hdrSize >> 1), mainVlcSize + hdrSize);
	    exifFillRotOpt = 0;
	    pbLogPrintf("pb edit suc\n");
    }
    else{
        printf("out of mem\n");
        ret = FAIL;       
    }
	
	return ret;
}

#if 1
#if 0
UINT8 pbEffect_Resize(PBFile_t* pFile, PBEffectList_t imgEffect, UINT16 dstWidth, UINT16 dstHeight)
{
	ExifImage_t mainImage;
	ExifImage_t thubImage;

	UINT8 ret = FAIL;
	UINT32 dramEndAddr = K_SDRAM_TotalSize - 256UL;//512 Bytes reserved for DMA
	UINT32 freeBuffAddr = K_SDRAM_ImagePlaybackBufAddr;
	UINT32 hdrAddr, fileAddr, decAddr;
	UINT32 hdrSize;
	UINT32 yuvBufInterval;
	UINT16 decW, decH;

	UINT32 effAddr, tmbEffAddr; 
	UINT32 sclTmpAddr;
	UINT32 vlcAddr, vlcSize;
	UINT32 mainVlcAddr, tmbVlcSize, mainVlcSize;
	UINT32 size;   //rhy++ 20100221
	UINT8 sizeFlag;
	UINT8 yuvType = 0x20;
    UINT8 option;
	UINT8 rotation;

    //xian ++ 20080624
	UINT8 qTableIndex;
	UINT8 qTable[128];
	UINT8 zigzagOpt = 1;
    //xian --

	//wang ++ 20080920
	UINT8 isResize=0x00;
	UINT16 RotThumbHeight = PB_DECODE_THB_HEIGHT;  //default value
	//wang-- 

	//printf("pb temp buf size=%ld\n",dramEndAddr-freeBuffAddr);
	//printf("dram end addr=0x%lx\n",dramEndAddr);
	//printf("dram free addr=0x%lx\n",freeBuffAddr);

	OrderToAddr(freeBuffAddr, 16);
    //printf("buf: 0x%lx - 0x%lx\n", freeBuffAddr, dramEndAddr);

    fileAddr = pbEditFileLoad(pFile, &mainImage, &thubImage, NULL);
    if(!fileAddr){
        return FAIL;
    }
	if(imgEffect>=PB_IMG_EFFECT_MAX+SPE_DISTORT_MAX)
	{
		if(dstWidth==mainImage.width&&dstHeight==mainImage.height){
			isResize = 0x02;   //rotation 
		}
		else{
			isResize=0x01;  //resize
		}
		//printf("\nresize\n");
	}
    //alloc buffer
	//yuvBufInterval = PB_EDIT_CDSP_DO_LINE_NUM * ((MainImgPara.Width>1024)?(UINT32)MainImgPara.Width:1024UL); // reserved for overlay
	yuvBufInterval = (UINT32)PB_EDIT_CDSP_DO_LINE_NUM * ((mainImage.width>1024)?(UINT32)mainImage.width:1024UL); // reserved for overlay

	//printf("yuvBufInterval=%ld\n",yuvBufInterval);
	
	hdrSize = (PB_EDIT_IMG_HDR_SIZE * 2UL);
	
	//decAddr=  (freeBuffAddr + yuvBufInterval+ (UINT32)96000);//320*240+160*120); //PB_DECODE_QV_WIDTH*PB_DECODE_QV_HEIGHT + PB_DECODE_THB_WIDTH*PB_DECODE_THB_HEIGHT );
	//decAddr=  (freeBuffAddr + yuvBufInterval+ (UINT32)PB_DECODE_QV_WIDTH*PB_DECODE_QV_HEIGHT + (UINT32)PB_DECODE_THB_WIDTH*PB_DECODE_THB_HEIGHT );

	if (yuvBufInterval> ((UINT32)PB_DECODE_QV_WIDTH*PB_DECODE_QV_HEIGHT*2 + (UINT32)PB_DECODE_THB_WIDTH*PB_DECODE_THB_HEIGHT)	){
		decAddr=  freeBuffAddr + yuvBufInterval+ (hdrSize>>1);
	}
	else{
		//decAddr=  freeBuffAddr + (hdrSize>>1) + ((UINT32)PB_DECODE_QV_WIDTH*PB_DECODE_QV_HEIGHT*2 + (UINT32)PB_DECODE_THB_WIDTH*PB_DECODE_THB_HEIGHT);
		decAddr=  freeBuffAddr + (hdrSize>>1) + ((UINT32)PB_DECODE_QV_WIDTH*PB_DECODE_QV_HEIGHT + (UINT32)PB_DECODE_THB_WIDTH*PB_DECODE_THB_HEIGHT);
	}

	OrderToAddr(decAddr, 16);
	//printf("freeBuffAddr=0x%lx\n",freeBuffAddr);
	//printf("decAddr=0x%lx\n",decAddr);
	
    //save Qtable, xian ++ 20080624
    jpegQTableGet(&qTableIndex, qTable, zigzagOpt);
	
	if(Rotation_En){                   //add by rhy 20100328,add the function of really rotation.
		rotation = pbRotModeGet(1);
		yuvType |= (rotation<<2);
		option = rotation;
	}
	else{
		yuvType = 0x20;
		option = 0;   //default value;
	}

	//decode main
	if(isResize == 1){     // Resize function
		decW=dstWidth;
		decH=dstHeight;
	}
	else if(isResize == 2){  // rotation function,
		if(Rotation_En){
			decW = MCU_ALIGN(mainImage.width, 16);
			decH = MCU_ALIGN(mainImage.height, 16);
			dstWidth = MCU_ALIGN(dstWidth,16);
			dstHeight = MCU_ALIGN(dstHeight, 16);   //keep MCU Align;
		}
	}
	else{					//effect function
		decW=mainImage.width;
		decH=mainImage.height;
		dstWidth=decW;
		dstHeight=decH;
	}
		
	
	/*********************/
	/* decode Main YUV	 */ 	
	/*********************/ 		
	//jpegQTableSet(0, qTable, qTable+64, zigzagOpt);   //xian ++ 20080624
	
	ret = pbEditDec(&mainImage, decAddr, dramEndAddr, &decW, &decH);
	if(ret){
		DBG_pbFuncTrack();
		return FAIL;
	}
	
	exifVlcAtOdd = 0;
	
	
	//printf("main w=%d,h=%d\n",decW,decH);
	//printf("main yuv addr=0x%lx\n",decAddr);
	//dramFileSave("PBMAIN00.YUV", decAddr, (UINT32)decW*decH*2);
	
	/*********************/
	/* Do QV YUV		 */ 	
	/*********************/ 		
	//HAL_DramCodeSentry(decAddr,decAddr+(UINT32)decW*decH*2,K_SDRAM_CodeSentryRedirectAddr);
	
	effAddr = (freeBuffAddr + (hdrSize>>1));
	OrderToAddr(effAddr,16);
	
	if(isResize){
		pbCdspEffectDo(PB_IMG_EFFECT_NORMAL, decAddr, decW, decH, effAddr, PB_DECODE_QV_WIDTH, PB_DECODE_QV_HEIGHT); 
	}			
	else{
		pbCdspEffectDo(imgEffect	 	   , decAddr, decW, decH, effAddr, PB_DECODE_QV_WIDTH, PB_DECODE_QV_HEIGHT); 
	}		
	//printf("qv addr=0x%lx\n",effAddr);
	//dramFileSave("PBQV0000.YUV", effAddr, (UINT32)PB_DECODE_QV_WIDTH*PB_DECODE_QV_HEIGHT*2);

	//dramFileSave("PBMAIN01.YUV", decAddr, (UINT32)decW*decH*2);	

	/*********************/
	/* Do thumbnail YUV	 */ 	
	/*********************/ 
	thubImage.width = PB_DECODE_THB_WIDTH;
	thubImage.height = PB_DECODE_THB_HEIGHT;

	tmbEffAddr = effAddr + (UINT32)PB_DECODE_QV_WIDTH * (UINT32)PB_DECODE_QV_HEIGHT;


	#if 0	/*cdsp redo thumbnail yuv*/
		if(isResize){	
			pbCdspEffectDo(PB_IMG_EFFECT_NORMAL, decAddr, decW, decH, tmbEffAddr, PB_DECODE_THB_WIDTH, PB_DECODE_THB_HEIGHT); 
		}
		else{
			pbCdspEffectDo(imgEffect		   , decAddr, decW, decH, tmbEffAddr, PB_DECODE_THB_WIDTH, PB_DECODE_THB_HEIGHT); 
		}
	
	#else	/* GPRM scale down from qv to thumbnail */
		sclTmpAddr = tmbEffAddr + (UINT32)PB_DECODE_THB_WIDTH * (UINT32)PB_DECODE_THB_HEIGHT;

		if(sclTmpAddr + (UINT32)thubImage.width * (UINT32)thubImage.height > mainImage.addr){
			DBG_pbFuncTrack();
			return FAIL;
		}
		if(Rotation_En){
			if(rotation&0x1){
				RotThumbHeight = 128;
			}
		}
		//printf("RotThumbHeight=%d\n",RotThumbHeight);
		
		HAL_GprmScale(
			effAddr,
			PB_DECODE_QV_WIDTH, PB_DECODE_QV_HEIGHT,
			tmbEffAddr,
			PB_DECODE_THB_WIDTH, RotThumbHeight,
			//sclTmpAddr,
			freeBuffAddr,
			1
		);
	
	#endif
	
	//printf("thb addr= 0x%lx\n", tmbEffAddr);
	//dramFileSave("PBTHM001.YUV", tmbEffAddr, (UINT32)PB_DECODE_THB_WIDTH*RotThumbHeight*2);

	//dramFileSave("PBMAIN02.YUV", decAddr, (UINT32)decW*decH*2);	
	/*********************/
	/* generate QV VLC	 */ 	
	/*********************/
	//vlcAddr = freeBuffAddr; //temp
	exifQvQTableIdx = 85;
	do
	{	sizeFlag = FALSE; //init falg;
		vlcAddr = freeBuffAddr;
		jpegQTableSet(exifQvQTableIdx, NULL, NULL, 1); //FOR TEST  ==>50
		HAL_JpgZFactorSet( 100, 100, PB_DECODE_QV_WIDTH, PB_DECODE_QV_HEIGHT, PB_DECODE_QV_WIDTH, PB_DECODE_QV_HEIGHT, option);

		HAL_JpegEncode( effAddr, vlcAddr, 0, yuvType/*0x20*/, 1 );
		HAL_JpegReadEncodeSize( &vlcSize);

		
		exifQvVlcAddr = vlcAddr;
		exifQvVlcSize = vlcSize;
		exifQvXSize = PB_DECODE_QV_WIDTH;
		exifQvYSize = PB_DECODE_QV_HEIGHT;
		if(Rotation_En){
			if(rotation&0x1){
				exifQvXSize = PB_DECODE_QV_HEIGHT;
				exifQvYSize = PB_DECODE_QV_WIDTH;
			}
		}
		//printf("qv vlc: 0x%lx, 0x%lx\n", vlcAddr, vlcSize);
	 	//printf("qv vlc size=0x%lx\n",vlcSize);
	 	//printf("qv vlc addr=0x%lx\n",vlcAddr);
	 	
		//dramFileSave("PBMAIN03.YUV", decAddr, (UINT32)decW*decH*2);	
		/*********************/
		/* generate Tmb VLC  */ 	
		/*********************/    
		vlcAddr = vlcAddr + ((vlcSize + 1)>> 1) ;

		exifThbQTableIdx = 85;
		jpegQTableSet(85, NULL, NULL, 1);
		if(Rotation_En){		
			HAL_JpgZFactorSet( 100, 100, PB_DECODE_THB_WIDTH, RotThumbHeight, PB_DECODE_THB_WIDTH, RotThumbHeight, option );
		}
		else{
			HAL_JpgZFactorSet( 100, 100, PB_DECODE_THB_WIDTH, PB_DECODE_HEIGHT, PB_DECODE_THB_WIDTH, PB_DECODE_HEIGHT, option );
		}
		//HAL_JpgZFactorSet( 100, 100, PB_DECODE_THB_WIDTH, PB_DECODE_THB_HEIGHT, PB_DECODE_THB_WIDTH, PB_DECODE_THB_HEIGHT, option );

		HAL_JpegEncode( tmbEffAddr, vlcAddr, 0, yuvType/*0x20*/, 1 );
		HAL_JpegReadEncodeSize( &vlcSize );

		//printf("Tmb vlc: 0x%lx, 0x%lx\n", vlcAddr, vlcSize);
		exifThbVlcAddr = vlcAddr;
		exifThbVlcSize = vlcSize;
		exifThbXSize = PB_DECODE_THB_WIDTH;
		exifThbYSize = RotThumbHeight;
		if(Rotation_En){
			if(rotation&0x1){
				exifThbXSize = RotThumbHeight;
				exifThbYSize = PB_DECODE_THB_WIDTH;
			}
		}
		//if(yuvType == 0x21){//temp for bug
			//exifThbXSize = 0;
			//exifThbYSize = 0;
		//}    
		tmbVlcSize = vlcSize;
		if( tmbVlcSize & 0x00000001 ) {
			size = ( K_EXIF_STRUCTURE0_SIZE + tmbVlcSize + K_EXIF_STRUCTURE3_SIZE + K_EXIF_STRUCTURE4_SIZE );
		}
		else {
			size = ( K_EXIF_STRUCTURE0_SIZE + tmbVlcSize + K_EXIF_STRUCTURE2_SIZE + K_EXIF_STRUCTURE4_SIZE );
		}
		if( 65536 - size< ( (UINT32)K_HDR2_SIZE + exifQvVlcSize ) ) {
			//printf( "qv size too large\n" );
			sizeFlag = TRUE;
			exifQvQTableIdx-=10;
		}
	
	}while(sizeFlag);
 	//printf("thm vlc size=0x%lx\n",tmbVlcSize);
 	//printf("thm vlc addr=0x%lx\n",vlcAddr);
 	//dramFileSave("PBMAIN04.YUV", decAddr, (UINT32)decW*decH*2);	
	/*********************/
	/* Do Main YUV		 */ 	
	/*********************/ 		
	
	//do main effect
	//effAddr = ((decAddr - (UINT32)yuvBufInterval) + 16 &~15);
	
	//effAddr = tmbEffAddr + PB_DECODE_THB_WIDTH*PB_DECODE_THB_HEIGHT;		
	//effAddr = decAddr -(UINT32)yuvBufInterval;
	effAddr = freeBuffAddr + (UINT32)64*1024; //modified by rhy,20110406 mantis #42329
	OrderToAddr(effAddr,16);
	//effAddr = effAddr + 16 & ~15;
	
	//wang ++ 20080916
	if(isResize){
		effAddr=decAddr;
	}			
	else{
		pbCdspEffectDo(imgEffect, decAddr, decW, decH, effAddr, decW, decH); 
	}		

	//printf("effer src=0x%lx,dst=0x%lx\n",decAddr,effAddr);
	//dramFileSave("PBMAIN05.YUV", effAddr, (UINT32)decW*decH*2);
	
	//move to end buf, xian ++ 20080626
	effAddr = pbEditImgMoveBufEnd(effAddr, decW, decH, dramEndAddr);
	//dramFileSave("PBMAIN06.YUV", effAddr, (UINT32)decW*decH*2);


	//encode to jpeg
	mainVlcAddr = freeBuffAddr + (hdrSize >> 1);
	
	exifMainQTableIdx = PB_EDIT_IMG_QTAB;
	jpegQTableSet(PB_EDIT_IMG_QTAB, NULL, NULL, 1);//
	//		  exifMainQTableIdx = 100;	//should not be zero
	//		  jpegQTableSet(0, qTable, qTable+64, zigzagOpt);	//xian ++ 20080624
	HAL_JpgZFactorSet( 100, 100, decW, decH, dstWidth, dstHeight, option );
	HAL_JpegEncode( effAddr, mainVlcAddr, 0, yuvType/*0x20*/, 1 );
	HAL_JpegReadEncodeSize( &mainVlcSize);
	//printf("main Vlc addr=0x%lx,main vlc size=0x%lx\n",mainVlcAddr,mainVlcSize);	
	
	//if( MainImgPara.DataType == K_File_DataType_YUV422 ) {
	//	exifFileFormat = 0x00;
	//}
	//else {
	//	exifFileFormat = 0x01;
	//}

	exifFileFormat = 0x00;
	exifMainXSize = dstWidth;
	exifMainYSize = dstHeight;
	if(Rotation_En){
		if(rotation&0x01){
			exifMainXSize = dstHeight;
			exifMainYSize = dstWidth;
		}
	}
    //make file
	hdrAddr = dramEndAddr - PB_EDIT_IMG_HDR_SIZE;
    if(hdrAddr > mainVlcAddr + mainVlcSize){
		#if (DBG_Level > 1)  		
        	DEBUG_PRINT("org exif ort: %ld\n", exifRotOpt);
		#endif //(DBG_Level > 1) 
	if(Rotation_En){
			exifRotOpt = 0;
	}
	else{
		exifRotOpt = (UINT32)pbExifOrientGet();
	}
    // printf("exif rotopt=%ld\n",exifRotOpt);   
		#if (DBG_Level > 1)  
        	DEBUG_PRINT("modify exif ort: %ld\n", exifRotOpt);
		#endif //(DBG_Level > 1) 
        exifFillRotOpt = 1;
		
    	/*EXIFHDR_Initialize_AllExifHeader(hdrAddr);
    	hdrSize = EXIFHDR_LoadExifHeader( mainVlcAddr, hdrAddr, tmbVlcSize );*/
    	//new exif lib
		hdrSize = exifCreateFile(mainVlcAddr,0,exifThbVlcAddr,tmbVlcSize);
		
    	ret = pbEditFileMake(mainVlcAddr - (hdrSize >> 1), mainVlcSize + hdrSize);
        exifFillRotOpt = 0;
		#if (DBG_Level > 1) 
        	printf("pb effect suc\n");
		#endif
    }
    else{
		#if (DBG_Level > 1) 
        	printf("out of mem\n");
		#endif
        DBG_pbFuncTrack();
        ret = FAIL;       
    }
	return ret;
}
#endif


#else
//WANG ++
UINT8 pbEffect_Resize(PBFile_t* pFile, PBEffectList_t imgEffect, UINT16 dstWidth, UINT16 dstHeight)
{
	UINT8 ret = FAIL;
	UINT32 dramEndAddr = K_SDRAM_TotalSize - 256UL;//512 Bytes reserved for DMA
	UINT32 freeBuffAddr = K_SDRAM_ImagePlaybackBufAddr;
	UINT32 hdrAddr, fileAddr, decAddr;
	UINT32 hdrSize;
	UINT32 yDis;
    UINT16 decW, decH;

    UINT32 effAddr, tmbEffAddr; 
    UINT32 sclTmpAddr;
    UINT32 vlcAddr, vlcSize;
    UINT32 mainVlcAddr, tmbVlcSize, mainVlcSize;

    UINT8 yuvType = 0x20;
    
	struct Exif_Decompress_Parameter MainImgPara;
	struct Exif_Decompress_Parameter TmbImgPara;

    //xian ++ 20080624
	UINT8 qTableIndex;
	UINT8 qTable[128];
	UINT8 zigzagOpt = 1;
    //xian --

	//wang ++ 20080920
	UINT8 isResize=0x00;
	//wang-- 

	//printf("pb temp buf size=%ld\n",dramEndAddr-freeBuffAddr);
	printf("dram end addr=0x%lx\n",dramEndAddr);
	
	if(imgEffect>=PB_IMG_EFFECT_MAX+SPE_DISTORT_MAX)
	{
		isResize=0x01;
		//printf("\nresize\n");
	}

	OrderToAddr(freeBuffAddr, 16);
    //printf("buf: 0x%lx - 0x%lx\n", freeBuffAddr, dramEndAddr);

    fileAddr = pbEditFileLoad(pFile, &MainImgPara, &TmbImgPara, NULL);
    if(!fileAddr)
        return FAIL;

		
    //alloc buffer
	yDis = PB_EDIT_CDSP_DO_LINE_NUM * ((MainImgPara.Width>1024)?(UINT32)MainImgPara.Width:1024UL); // reserved for overlay
	
	hdrSize = (PB_EDIT_IMG_HDR_SIZE * 2UL);
	decAddr= ((freeBuffAddr + (hdrSize >> 1) + yDis) + 16 &~15);
	printf("decAddr=0x%lx\n",decAddr);
	
    //save Qtable, xian ++ 20080624
    jpegQTableGet(&qTableIndex, qTable, zigzagOpt);

    //decode main to QV size
    decW = PB_DECODE_WIDTH;
    decH = PB_DECODE_HEIGHT;
  
    ret = pbEditDec(&MainImgPara, decAddr, MainImgPara.Addr, &decW, &decH);
    if(ret){
		DBG_pbFuncTrack();
        return FAIL;
    }
      

	//printf("vlc start=%lx\n",MainImgPara.Addr);
	
	//printf("start=%lx,end=%lx\n",decAddr,MainImgPara.Addr);
	
	//printf("w=%d,h=%d\n",decW,decH);
	
    //xian ++
    //if(!TmbImgPara.Width || !TmbImgPara.Height){
        TmbImgPara.Width = 160;
        TmbImgPara.Height = 120;
    //}
	/*if( MainImgPara.DataType == K_File_DataType_YUV422 ) {
        yuvType = 0x20;
    }
    else{
        yuvType = 0x21;
    }*/
    //xian --

	//do QV effect
	//effAddr = ((decAddr - (UINT32)yDis) + 16 &~15);
	effAddr = ((decAddr - (UINT32)decW*decH) - 16 &~15);



   	if(isResize)
		effAddr=decAddr;
	else
		pbCdspEffectDo(imgEffect, decAddr, decW, decH, effAddr, decW, decH); 
		

  	//printf("eff start=%lx\n",decAddr);	
    //printf("end QV effect\n");
	//dramFileSave("PBQV0000.YUV", effAddr, (UINT32)320*240*2);
	
    //do tmb effect
    //tmbEffAddr = effAddr + (UINT32)decW * (UINT32)decH);
    tmbEffAddr = effAddr + (UINT32)TmbImgPara.Width * (UINT32)TmbImgPara.Height;
    sclTmpAddr = tmbEffAddr + (UINT32)TmbImgPara.Width * (UINT32)TmbImgPara.Height;
    //printf("tmbEffAddr = 0x%lx, sclTmpAddr= 0x%lx\n", tmbEffAddr, sclTmpAddr);

    if(sclTmpAddr + (UINT32)decW * (UINT32)TmbImgPara.Height > MainImgPara.Addr){
		DBG_pbFuncTrack();
		return FAIL;
    }
    
    HAL_GprmScale(
    	effAddr,
    	decW, decH,
    	tmbEffAddr,
    	TmbImgPara.Width, TmbImgPara.Height,
    	sclTmpAddr,
    	1
    );

	//dramFileSave("PBTHM000.YUV", tmbEffAddr, (UINT32)160*120*2);
	
    /*********************/
    /* generate QV VLC	 */	    
    /*********************/
	vlcAddr = freeBuffAddr; //temp
    
	exifQvQTableIdx = 85;
	jpegQTableSet(85, NULL, NULL, 1);
	HAL_JpgZFactorSet( 100, 100, decW, decH, PB_DECODE_WIDTH, PB_DECODE_HEIGHT, 0x00 );

	HAL_JpegEncode( effAddr, vlcAddr, 0, yuvType/*0x20*/, 1 );
	HAL_JpegReadEncodeSize( &vlcSize);

    //printf("Qv vlc: 0x%lx, 0x%lx\n", vlcAddr, vlcSize);
	exifQvVlcAddr = vlcAddr;
	exifQvVlcSize = vlcSize;
	exifQvXSize = PB_DECODE_WIDTH;
	exifQvYSize = PB_DECODE_HEIGHT;

    
    /*********************/
    /* generate Tmb VLC	 */	    
    /*********************/    
    vlcAddr = vlcAddr + ((vlcSize + 1)>> 1) ;
    
	exifThbQTableIdx = 85;
	jpegQTableSet(85, NULL, NULL, 1);
	HAL_JpgZFactorSet( 100, 100, TmbImgPara.Width, TmbImgPara.Height, TmbImgPara.Width, TmbImgPara.Height, 0x00 );

	HAL_JpegEncode( tmbEffAddr, vlcAddr, 0, yuvType/*0x20*/, 1 );
	HAL_JpegReadEncodeSize( &vlcSize );
    
    //printf("Tmb vlc: 0x%lx, 0x%lx\n", vlcAddr, vlcSize);
  	exifThbVlcAddr = vlcAddr;
    exifThbVlcSize = vlcSize;
	exifThbXSize = TmbImgPara.Width;
	exifThbYSize = TmbImgPara.Height;
    //if(yuvType == 0x21){//temp for bug
		//exifThbXSize = 0;
		//exifThbYSize = 0;
    //}    
    tmbVlcSize = vlcSize;

    //decode main
    if(isResize)
    {
    	decW=dstWidth;
		decH=dstHeight;
    }
	else
	{
		decW=MainImgPara.Width;
		decH=MainImgPara.Height;
		dstWidth=decW;
		dstHeight=decH;
    }
	
/*    if(decW == PB_DECODE_WIDTH && decH == PB_DECODE_HEIGHT)
    {//just use Qv
    	exifVlcAtOdd = 0;

        mainVlcAddr = exifQvVlcAddr;
    	exifMainQTableIdx = PB_EDIT_IMG_QTAB;
        mainVlcSize = exifQvVlcSize;        
    }
    else*/
    

	    //fileAddr = pbEditFileLoad(pFile, &MainImgPara, &TmbImgPara, NULL);


    /*********************/
    /* decode Main YUV   */	    
    /*********************/  	    
        jpegQTableSet(0, qTable, qTable+64, zigzagOpt);   //xian ++ 20080624

        ret = pbEditDec(&MainImgPara, decAddr, dramEndAddr, &decW, &decH);
        if(ret){
    		DBG_pbFuncTrack();
            return FAIL;
        }    
    	exifVlcAtOdd = 0;



		//dramFileSave("PBMAIN01.YUV", decAddr, (UINT32)decW*decH*2);

		//printf("main h=%d,v=%d\n",decW,decH);

	/*********************/
	/* Do Main YUV   	 */ 	
	/*********************/ 		

        //do main effect
    	effAddr = ((decAddr - (UINT32)yDis) + 16 &~15);
    	
		
    	//wang ++ 20080916
		if(isResize)
			effAddr=decAddr;
		else
    		pbCdspEffectDo(imgEffect, decAddr, decW, decH, effAddr, decW, decH); 
		
	
		//wang --
		
     
        //move to end buf, xian ++ 20080626
        effAddr = pbEditImgMoveBufEnd(effAddr, decW, decH, dramEndAddr);

        //encode to jpeg
    	mainVlcAddr = freeBuffAddr + (hdrSize >> 1);

    	exifMainQTableIdx = PB_EDIT_IMG_QTAB;
    	jpegQTableSet(PB_EDIT_IMG_QTAB, NULL, NULL, 1);//
//        exifMainQTableIdx = 100;  //should not be zero
//        jpegQTableSet(0, qTable, qTable+64, zigzagOpt);   //xian ++ 20080624
    	HAL_JpgZFactorSet( 100, 100, decW, decH, dstWidth, dstHeight, 0x00 );
    	HAL_JpegEncode( effAddr, mainVlcAddr, 0, yuvType/*0x20*/, 1 );
    	HAL_JpegReadEncodeSize( &mainVlcSize);
        

	//if( MainImgPara.DataType == K_File_DataType_YUV422 ) {
	//	exifFileFormat = 0x00;
	//}
	//else {
	//	exifFileFormat = 0x01;
	//}
	exifFileFormat = 0x00;
	exifMainXSize = dstWidth;
	exifMainYSize = dstHeight;
    
    //make file
	hdrAddr = dramEndAddr - PB_EDIT_IMG_HDR_SIZE;
    if(hdrAddr > mainVlcAddr + mainVlcSize){
		#if (DBG_Level > 1)  		
        	DEBUG_PRINT("org exif ort: %ld\n", exifRotOpt);
		#endif //(DBG_Level > 1) 
        exifRotOpt = (UINT32)pbExifOrientGet();
		#if (DBG_Level > 1)  
        	DEBUG_PRINT("modify exif ort: %ld\n", exifRotOpt);
		#endif //(DBG_Level > 1) 
        exifFillRotOpt = 1;
    	EXIFHDR_Initialize_AllExifHeader(hdrAddr);
    
    	hdrSize = EXIFHDR_LoadExifHeader( mainVlcAddr, hdrAddr, tmbVlcSize );
    	
    	ret = pbEditFileMake(mainVlcAddr - (hdrSize >> 1), mainVlcSize + hdrSize);
        exifFillRotOpt = 0;
        printf("pb effect success\n");
    }
    else{
        printf("out of memory\n");
        DBG_pbFuncTrack();
        ret = FAIL;       
    }

	return ret;
}
#endif
#endif

#if (STILL_LIB_PB_DMY)
UINT8 pbEffectSet(PBFile_t* pFile, pbEditType_t imgEffect)
{
        pFile=pFile;
        imgEffect=imgEffect;
        return 0;
}
#else
UINT8 pbEffectSet(PBFile_t* pFile, pbEditType_t imgEffect)
{
	UINT8 dummy = 0;	   /* avoid XDATA overflow ,don't modify*/
	
	PbCropImage_t cropRec;
	cropRec.srcWidth = 0;
	cropRec.srcHeight = 0;
	
	 /* add for avoid XDATA overflow  */
	if(dummy == 1) {
		speDistortSave(0, 0);  
	}
	
	pbEdit_ImgDec(pFile,PB_IMG_EFFECT, &cropRec);
	return pbEdit_Save(imgEffect, cropRec);
	//return pbEffect_Resize(pFile,imgEffect,0,0);
}
#endif

#if (STILL_LIB_PB_DMY)
UINT8 pbImgResize(PBFile_t* pFile, UINT16 dstWidth, UINT16 dstHeight)
{
        pFile=pFile;
        dstWidth=dstWidth;
        dstHeight=dstHeight;
        return 0;
}
#else
UINT8 pbImgResize(PBFile_t* pFile, UINT16 dstWidth, UINT16 dstHeight)
{	
	UINT8 ret;
	
	PbCropImage_t cropRec;
	cropRec.srcWidth = dstWidth;
	cropRec.srcHeight = dstHeight;
	
	pbEdit_ImgDec(pFile,PB_RESIZE, &cropRec);
	ret = pbEdit_Save(PB_RESIZE,cropRec);
	
	return ret;
}
#endif

//viviOffset should not exceed one sector
UINT8 pbEditUserCommentCodeModify(PBFile_t* pFile,  UINT8 *cmCode, UINT8 len)
{
    UINT8 ret = FAIL;
#if 0 //xian -- 20110809 temp for code size
	pFile = pFile;
	cmCode = cmCode;
	len = len;	
#else
    UINT32 fileAddr = K_SDRAM_ImagePlaybackBufAddr;
    UINT32 offset, startPos;
    UINT8 buf[2];
    UINT8 fno;

    if(pFile->FileType == SP1K_PB_FILE_TYPE_JPG){
		offset = exifIFDTagOffsetGet(fileAddr,EXIF_IFD_EXIF,EXIF_TAG_SUBJECT_AREA,0);
		offset += 12;
		//printf("^^:%u, %u\n", ( UINT16 ) hd0->APP1.Exif_SubjectArea, ( UINT16 ) hd0->SOI.Marker);
    }
    else if(pFile->FileType == SP1K_PB_FILE_TYPE_AVI){
        offset = 0x1F0;
    }

    startPos = offset & ~(UINT32)511;    
  //  printf("vi=%lx, %lx\n", offset, startPos);
    offset = offset % 512;

    if(offset + len > 512){
        len = 512 - offset;
    }
   // printf("of: %lu, %bu\n", offset, len);
    
    fno = DOS_FileOpen(pFile->FileName, 'r');
    if(fno){
        DOS_FileRead(fno, fileAddr<<1, 512, startPos);
        ret = DOS_FileSeek(fno, startPos, 0);
        //printf("ret:%bu\n", ret);            

        if(offset & 1){ //first odd byte
            offset >>= 1;
            HAL_DramStartAddrSet( fileAddr+offset, 1 );
            HAL_DramWordDataRead(&buf[0], &buf[1]);

            buf[1] = *cmCode ++;
            len --;
           // printf("+%bx,%bx\n", buf[0],buf[1]);

            HAL_DramStartAddrSet( fileAddr+offset, 0 );
            HAL_DramWordDataWrite(buf[0], buf[1]);
            offset ++;
        }
        else{
            offset >>= 1;
            HAL_DramStartAddrSet( fileAddr+offset, 0 );
        }

        while(len>1){
            buf[0] = *cmCode ++;
            buf[1] = *cmCode ++;
            //printf("%bx,%bx\n", buf[0],buf[1]);
            HAL_DramWordDataWrite(buf[0], buf[1]);
            len -= 2;
            offset ++;
        }

        if(len){// last odd byte
            HAL_DramStartAddrSet( fileAddr+offset, 1 );
            HAL_DramWordDataRead(&buf[0], &buf[1]);

            buf[0] = *cmCode;
            //printf("-%bx,%bx\n", buf[0],buf[1]);
            
            HAL_DramStartAddrSet( fileAddr+offset, 0 );
            HAL_DramWordDataWrite(buf[0], buf[1]);
        }
                               
        ret = DOS_FileCurrSectorWrite(fno, fileAddr);        
        //printf("ret:%bu\n", ret);

        DOS_FileClose(fno);

        ret = SUCCESS;
    }
#endif    
    return ret;
}

UINT8 pbEditUserCommentCodeGet(PBFile_t* pFile,  UINT8 *cmCode, UINT8 len)
{
    UINT8 ret = FAIL;
#if 0 //xian -- 20110809 temp for code size
	pFile = pFile;
	cmCode = cmCode;
	len = len;
#else
    UINT32 fileAddr = K_SDRAM_ImagePlaybackBufAddr;
    UINT32 offset, startPos;
    UINT8 buf[2];
    UINT8 fno;
    UINT8 *pt = cmCode;
 
    if(pFile->FileType == SP1K_PB_FILE_TYPE_JPG){
        offset = exifIFDTagOffsetGet(fileAddr,EXIF_IFD_EXIF,EXIF_TAG_SUBJECT_AREA,0);
		offset += 12;
    }
    else if(pFile->FileType == SP1K_PB_FILE_TYPE_AVI){
        offset = 0x1F0;
    }

    startPos = offset & ~(UINT32)511;    
    //printf("vi=%lx, %lx\n", offset, startPos);
    offset = offset % 512;

    if(offset + len > 512)
        len = 512 - offset;
    //printf("of: %lu, %bu\n", offset, len);
    
    fno = DOS_FileOpen(pFile->FileName, 'r');
    if(fno){
        DOS_FileRead(fno, fileAddr<<1, 512, startPos);
        DOS_FileClose(fno);

        if(offset & 1){ //first odd byte
            offset >>= 1;
            HAL_DramStartAddrSet( fileAddr+offset, 1 );
            HAL_DramWordDataRead(&buf[0], &buf[1]);

            *cmCode ++ = buf[1];
            len --;

            offset ++;
        }
        else{
            offset >>= 1;
            HAL_DramStartAddrSet( fileAddr+offset, 1 );
        }

        while(len>1){
            HAL_DramWordDataRead(&buf[0], &buf[1]);
            *cmCode ++ = buf[0];
            *cmCode ++ = buf[1];

            len -= 2;
            offset ++;
        }

        if(len){// last odd byte
            HAL_DramStartAddrSet( fileAddr+offset, 1 );
            HAL_DramWordDataRead(&buf[0], &buf[1]);
            *cmCode ++ = buf[0];
        }
    
        ret = SUCCESS;
    }
#endif
    return ret;
}

