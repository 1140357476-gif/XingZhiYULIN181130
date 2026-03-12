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
 * @file	  pb_decode.c
 * @brief	  still picture play back base function
 * @author	  Tq.zhang
 * @since	  2010-05-06
 * @date	  2010-05-06
*/
/**************************************************************************
 *						  H E A D E R	F I L E S
 **************************************************************************/
#include "general.h"
#include "initio.h"
#include "pb_general.h"
#include "pb_api.h"
//#include "dosusr.h"
//#include "exifhdr.h"
#include "exiffunc.h"
#include "exifvar.h"
#include "doslink.h"
#include "cardlink.h"
#include "disp_api.h"
#include "sp1k_snap_api.h"
#include "pb_debug.h"
#include "os_msg.h"
#include "hal_jpeg.h"
#include "hal_disp.h"
//wangjg++
#include "sdramhw.h"
#include "hal.h"
#include "hal_dram.h"
#include "hal_gprm.h"
#include "hal_global.h"
#include "snap_qtable.h"
#include "vfs_api.h"
#include "dcf_api.h"
#include "gprm.h"
#include "utility.h"
#include "os_api.h"
#include "video_debug.h"
#include "Sp1k_pb_api.h"   //rhy++
#include "reg_def.h"

#include "exif_def.h"
#include "exif_entry.h"
//#include "dbg_mem.h"
/**************************************************************************
 *							 C O N S T A N T S							  *
 **************************************************************************/
 
/**************************************************************************
 *								M A C R O S 							  *
 **************************************************************************/
#if (!STILL_LIB_PB_DMY)
#define PB_FILL_BLACK		1
#define PB_REAL_ROTATION_SUPPORT 0
/*
#if 0
#define PB_CHECK_ABORT()	if(osMsgSeek(SP1K_MSG_KEY_PRESS_LEFT) ||osMsgSeek(SP1K_MSG_KEY_PRESS_RIGHT)) {\
								return FAIL;\
							}
#else
#define PB_CHECK_ABORT()
#endif
*/
#if 0
#define debug_prt	printf
#define debug_prt
#endif

/**************************************************************************
 *							D A T A    T Y P E S						  *
 **************************************************************************/
//wang ++ 20081016
RECT16 g_pb_rec_RotOut;

/**************************************************************************
 *						   G L O B A L	  D A T A						  *
 **************************************************************************/
UINT8 xdata imgScaleFac;

static UINT8 xdata pbDispBuf = PB_DISP_BUF_A;
static UINT16 xdata pbDecodeW = PB_DECODE_WIDTH;
static UINT16 xdata pbDecodeH = PB_DECODE_HEIGHT;
static UINT8 xdata pbDispBufMode = PB_DISP_BUF_MODE_INIT;
static UINT16 xdata pbDispQVW = PB_DECODE_WIDTH;
static UINT16 xdata pbDispQVH = PB_DECODE_HEIGHT;
/**************************************************************************
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/
//tq ++
extern UINT32 videoPbReadDword(UINT32 dramAddr);

//wang++
extern xdata UINT8 exifFillRotOpt;
//wang--
//extern UINT8 xdata NeedReadMoreHeader;
extern xdata UINT8 exifVlcAtOdd;
extern xdata UINT8 exifQvVlcAtOdd;
extern xdata UINT8 exifThbVlcAtOdd;
//xian ++ 20080602
extern xdata UINT32 exifQvYTblAddr;
extern xdata UINT32 exifQvUVTblAddr;
//xian --
extern xdata UINT8 exifAppN;
extern xdata UINT16 G_JpgRstMcuNum;

//xian ++ 20080926
extern PBFile_t xdata MediaInfo;
extern UINT32 aviFileLength;
extern xdata UINT8 isStdHufTab;

//xian ++ 20121010
extern UINT16 exifMainXSize;
extern UINT16 exifMainYSize;

//xian -- 20110214
//SY add for HD play on HDMI s
//UINT16 decW_HDMI=320;
//UINT16 decH_HDMI=240;
//SY add for HD play on HDMI e
static UINT8 (*pbDecodeAbort)();  //rhy++ 
/**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/
/**
 * @fn		 UINT8 PbCheckAbort( UINT8 ImgType)
 * @brief	          Pb Abort
 * @param	  [in] ImagType : [0]:MainImage
                                               [1]:Thumbnail
                                               [2]:QV
 * @retval	  return = SUCCESS /FALSE
 * @see 	  NULL
 * @author	  hy.ren
 * @since	  2010-01-25
 * @todo	  N/A
 * @bug 	  N/A
*/

UINT8
pbCheckAbort( 
	UINT8 imgType,
	UINT16 line
)
{
	if((imgType != 0) || (!exifAppN)){
		return FALSE;     //if qv & thumbnail , not abort
	}

	if((*pbDecodeAbort)() == FALSE){
		return FALSE;	
	}
	else{
		printf("abort Line=%u\n",line);
		return TRUE;
	}
}

void
pbCallbackReg(
	UINT32 id,
	void *pfunct
)
{
	if( id == SP1K_PB_CALLBACK_ABORT) {
		pbDecodeAbort = pfunct;
	}
}


UINT8 
pbJpgDecodeSizeFit(
	UINT32 vlcAddr,
	UINT32 vlcSize,
	UINT16 imgW,
	UINT16 imgH,
	UINT32 dstAddr,
	UINT16 dstW,
	UINT16 dstH,
	UINT8 fillBlack
);
void pbDramDWordGet(UINT32 AddrWord,UINT32 AddrOff,UINT32 * DataDWord,UINT8 RstAddrEn,UINT8 order);
//=============================================================================
//Program
//=============================================================================

//-----------------------------------------------------------------------------
//pbImgDecode
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 pbImgDecode(PBFile_t* pFile, UINT32 DecAddrWord, UINT8 DecOpt)
 * @brief	  single picture image decode
 * @param	  [in] pFile : file information
 * @param	  [in] DecAddrWord : decode destination word address
 * @param	  [in] DecOpt : [0]:QVGA
 *						[1]:Thumbnail 
 *						[2] Full
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-6
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 pbImgDecode(PBFile_t* pFile, UINT32 decAddrWord, UINT8 decOpt) USING_0
{
	UINT8 sts = TRUE;
	UINT16 fHandle = TRUE;
	UINT32 decSize = 0;
	UINT8 bShowThumb = 0;
	UINT16 decWidth = PB_DECODE_WIDTH;
	UINT16 decHeight = PB_DECODE_HEIGHT;
	UINT32 srcAddr;
	UINT32 dramEndAddr = K_SDRAM_TotalSize - 257UL;//512 Bytes reserved for DMA, 2 Bytes for odd file size
	UINT16 i;
	UINT32 filesize;
	UINT8 isQvShow = 1;
	UINT16 exifRot;
	
	ExifImage_t mainImage;
	ExifImage_t qvImage;
	ExifImage_t thumbImage;

	if(pFile == NULL || decAddrWord > dramEndAddr) {
		return FAIL;
	}

	fHandle =  vfsFileOpen(pFile->FileName, K_DOS_ReadFile, pFile->FileStartCluster);
	if (!fHandle) {
		DBG_pbFuncTrack();
		return FAIL;
	}

	filesize = vfsFileSizeGet(fHandle);
	if (decOpt & PB_DECODE_OPT_FULL) {
		decSize = filesize;
	} 
	else{
	  	if (decOpt & PB_DECODE_OPT_THUMB) {
			bShowThumb = 0;
		}
		
		decSize = (UINT32)64*1024;
		if(decSize > filesize){
			decSize = filesize;
		}
	}
	if ((decSize >> 1) > (dramEndAddr - decAddrWord)) {
		vfsFileClose(fHandle, pFile->FileName, K_DOS_ReadFile, 0);
		DBG_pbFuncTrack();
		return FAIL;
	}
	
	srcAddr = dramEndAddr - (decSize >> 1);
	pbDecodeSizeSet(decWidth, decHeight);
	sts = vfsFileRead(fHandle, K_DOS_DstDram, decSize, srcAddr, 0);

	if (sts != TRUE) {
		vfsFileClose(fHandle, pFile->FileName, K_DOS_ReadFile, 0);
		DBG_pbFuncTrack();
		return FAIL;
	}
	sts = exifParserStart(exifGetMainInfo,&mainImage,srcAddr,decSize);
	exifRot = (UINT32)mainImage.orientation;
	if((mainImage.width==3648)||(mainImage.width==1920)){
		isQvShow = 0;
		exifAppN = 0;
	}
	
	/*Check if read enough header*/
	if (exifAppN && (sts == FALSE) && ((decSize<filesize) || (bShowThumb)) && (dispDevActGet() !=4)) {	 // for 320x240 JPG
	    srcAddr = dramEndAddr - ((filesize >> 1)>> 1);
		sts = vfsFileRead(fHandle,K_DOS_DstDram,(filesize >> 1),srcAddr,0);	 //Extract only 1/2 of the whole file

		if(sts){
			sts = exifParserStart(exifGetQVInfo,&qvImage,srcAddr,filesize >> 1);
		}
		
	}
	else if(((!exifAppN || !isQvShow) && decSize < filesize)){//xian ++ fix 28019
		srcAddr = dramEndAddr - (filesize>> 1);
		if(srcAddr <= decAddrWord){
			vfsFileClose(fHandle,  pFile->FileName, K_DOS_ReadFile, 0);
			DBG_pbFuncTrack();
			return FAIL;
		}
		sts = vfsFileRead(fHandle,K_DOS_DstDram,filesize,srcAddr,0);
		if (sts) {			
			sts = exifParserStart(exifGetThumbInfo,&thumbImage,srcAddr,filesize);
			sts = exifParserStart(exifGetMainInfo, &mainImage, srcAddr, filesize);
		}
	}

	vfsFileClose(fHandle,  pFile->FileName, K_DOS_ReadFile, 0);
	if (sts != TRUE) {
		DBG_pbFuncTrack();
		return FAIL;
	}

	pFile->MediaWidth = mainImage.width;
	pFile->MediaHeight = mainImage.height;

	pbLogPrintf("w is %u, h is %u \n", mainImage.width, mainImage.height);
	pbLogPrintf("w is %u, h is %u \n", thumbImage.width, thumbImage.height);

	//xian ++ 20080603
	pbLogPrintf("exifRotOpt = %ld\n", exifRot);
	pbParamSet(PB_PARAM_EXIF_ORIENT, (UINT16)mainImage.orientation);
	pbLogPrintf("decOpt=%bu, %bu\n", decOpt, exifAppN);

	sts = SUCCESS;
	
	if ((decOpt & PB_DECODE_OPT_QVGA) && exifAppN && isQvShow ) {
		UINT8 xdata qTable[128];
		sts = exifParserStart(exifGetQVInfo,&qvImage,srcAddr,filesize);
		qvImage.dataType = mainImage.dataType;
		
		SDRAM_CardSetStartAddress( exifQvYTblAddr, TRUE );
		if (exifQvVlcAtOdd){
			UINT8 dummy;
			M_DRAM_ReadWord(dummy, qTable[0]);
			for (i = 1; i < 127; i += 2){
				M_DRAM_ReadWord(qTable[i], qTable[i+1]);
			}
			M_DRAM_ReadWord(qTable[127], dummy);
		}else{
			for (i = 0; i < 128; i += 2){
				M_DRAM_ReadWord(qTable[i], qTable[i+1]);
			}
		}
		jpegQTableSet(0, qTable, (UINT8*)(qTable+64), 1);
		sts = pbImgDeCmp(&qvImage, decAddrWord, 2);
	} 
	else {	
		if (decOpt & PB_DECODE_OPT_THUMB) {
			sts = pbImgDeCmp(&thumbImage, decAddrWord, 1);
		}

		pbDispDevW_H_Get(&decWidth,&decHeight,mainImage.width,mainImage.height);
		if(decWidth > 960){
			decWidth = decWidth>>1;
			decHeight = decHeight>>1;
		}
		pbDecodeSizeSet(decWidth, decHeight); 
		sts = pbImgDeCmp(&mainImage, decAddrWord, 0);
	}
	HAL_GlobalCamModeSet(CAM_MODE_IDLE);

	return sts;
}

/**
 * @fn		  UINT8 pbImgDeCmp(Exif_Decompress_Parameter* ImagePara, UINT32 DecAddrWord, UINT8 ImgType)
 * @brief	  still image decompress
 * @param	  [in] ImagePara : main image parameters
 * @param	  [in] DecAddrWord : decode destination word address
 * @param	  [in] ImgType : '0' for Main, '1' for Thumbnail, '2' for QVGA
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-6
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 pbImgDeCmp(ExifImage_t *ImagePara, UINT32 decAddrWord, UINT8 imgType) USING_0
{
	UINT16 decWidth;
	UINT16 decHeight;
	UINT8 rotMode;
	Pb_Dec_Parameter decPara;

	pbDecodeSizeGet(&decWidth, &decHeight);
	rotMode = pbDecRotModeGet();	
	pbLogPrintf("rotMode=%bu\n", rotMode);	 
	
	decPara.DecBuff = decAddrWord;
	decPara.RotMode = rotMode;
	decPara.DecW = decWidth;
	decPara.DecH = decHeight;
	decPara.ImgType = imgType;

	if((rotMode & 4) && decWidth == 320){
		decPara.DecW = 320;
		decPara.DecH = 240;	
	}

	return pbImgDeCmpRot(ImagePara, &decPara, 0);
}
#endif
/**
 * @fn		  UINT32 pbDispBuffSwitch(void)
 * @brief	  switch display buffer flag, and return current active display buffer word address
 * @param	  NONE
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-6
 * @todo	  N/A
 * @bug 	  N/A
*/
#if (STILL_LIB_PB_DMY)
UINT32 pbDispBuffSwitch(void) USING_0
{
        return 0;
}
#else
UINT32 pbDispBuffSwitch(void) USING_0
{
	pbDispBuf = !pbDispBuf;

	pbDispBufMode = PB_DISP_BUF_MODE_AB; //xian ++

	return pbDispBuffGet(1);
}
#endif
/**
 * @fn		  UINT32 pbDispBuffGet(UINT8 opt)
 * @brief	  get display buffer address
 * @param	  [in] opt: '0' get inactive address, '1' get active address
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-6
 * @todo	  N/A
 * @bug 	  N/A
*/
#if (STILL_LIB_PB_DMY)
UINT32 pbDispBuffGet(UINT8 opt) USING_0
{
        opt=opt;
        return K_SDRAM_ImagePlaybackDisplayBufAddr;
}
#else
UINT32 pbDispBuffGet(UINT8 opt) USING_0
{
	if (opt) {//Active buffer
		if (pbDispBuf == PB_DISP_BUF_A) {
			return K_SDRAM_ImagePlaybackDisplayBufAddr;
		} else { //pbDispBuf == PB_DISP_BUF_B
			return (K_SDRAM_ImagePlaybackDisplayBufAddr + SP1K_PB_DISP_BUF_SIZE_B);
		}
	} else {//inactive buffer
		if (pbDispBuf == PB_DISP_BUF_A) {
			return (K_SDRAM_ImagePlaybackDisplayBufAddr + SP1K_PB_DISP_BUF_SIZE_B);
		} else { //pbDispBuf == PB_DISP_BUF_B
			return K_SDRAM_ImagePlaybackDisplayBufAddr;
		}
	}
}
#endif
/**
 * @fn		  UINT32 pbFreeBuffGet(void)
 * @brief	  get play back free buffer address
 * @param	  NONE
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-6
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT32 pbFreeBuffGet(void) USING_0
{
	return K_SDRAM_ImagePlaybackGeneralBufAddr;
}

/**
 * @fn		  void pbDecodeSizeGet(UINT16* Width, UINT16* Height)
 * @brief	  get decode width and height (pixel)
 * @param	  [in] Width :	pixel widht, pixel height
 * @param	  [in] Height
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-6
 * @todo	  N/A
 * @bug 	  N/A
*/
#if (STILL_LIB_PB_DMY)
void pbDecodeSizeGet(UINT16* Width, UINT16* Height) USING_0
{
	*Width = 320;
	*Height =240;
}
#else
void pbDecodeSizeGet(UINT16* Width, UINT16* Height) USING_0
{
	*Width = pbDecodeW;
	*Height = pbDecodeH;
}
#endif
/**
 * @fn		  void pbDecodeSizeSet(UINT16 Width, UINT16 Height)
 * @brief	  set decode width and height (pixel)
 * @param	  [in] Width : pixel width
 * @param	  [in] Height : pixel height
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-6
 * @todo	  N/A
 * @bug 	  N/A
*/
#if (STILL_LIB_PB_DMY)
void pbDecodeSizeSet(UINT16 Width, UINT16 Height) USING_0
{
	Width = Width;
	Height = Height;
}
#else
void pbDecodeSizeSet(UINT16 Width, UINT16 Height) USING_0
{
	pbDecodeW = Width;
	pbDecodeH = Height;
}
#endif

#define PBMAX(x,y)  ((x)>(y)) ? (x) : (y)

static UINT8 pbDecodeFactorGet(UINT32 bufSize,UINT16 srcWidth,UINT16 srcHeight,UINT16 dstWidh,UINT16 dstHeight,UINT8 opt)
{	
	UINT16 tmpW,tmpH;
	UINT8 fac,i;
	UINT32 decBuf,tmpBuf,sclBuf,useBuf;
	UINT8 decFacTable[4] = {0x08,0x04,0x02,0x01};

	for( i=0; i<4; i++ ) {
		fac = decFacTable[i];
		tmpW = ((srcWidth * (UINT16)fac) >> 3) ;
		tmpH = ((srcHeight * (UINT16)fac) >> 3);

		sclBuf = (UINT32)dstWidh *dstHeight;
		tmpBuf = (UINT32)(PBMAX(tmpW,dstWidh))*(UINT32)(PBMAX(tmpH,dstHeight));
		
		if(opt || (tmpW%8) || (opt&&(tmpH%8))){
			decBuf = (UINT32)tmpW * tmpH;
			if(decBuf>sclBuf){
				decBuf = decBuf<<1;
				sclBuf = 0;
			}
			
			useBuf = decBuf+sclBuf+tmpBuf;
			if(useBuf> bufSize ){
				if(i == 3){
					return 0xFF;
				}
				continue;
			}
		}
		else{
			decBuf = (UINT32)tmpW * tmpH;
		}
		useBuf = decBuf+sclBuf+tmpBuf;
		if(useBuf<bufSize){
			break;
		}
	}
	
	if( useBuf > bufSize) {	
		printf("L%u\n", (UINT16)__LINE__);
		return 0xFF;
	}
	
	if(fac == 8){
		fac = 0;
	}
	//printf("fac=%bu\n",fac);
	return fac;
}

UINT8 pbImgDecParaGet( pbImageDecPara_t *decImage,UINT8 fillBlack)
{
	UINT8 exifRotation;
	UINT8 realRotation;
	UINT8 xdata* pOddVlc;
	UINT32 endAddr;
	UINT32 dramEndAddr = K_SDRAM_TotalSize - 256UL;
	UINT16 decWidth,decHeight;
	UINT32 decBuf;
	//UINT8 ratX,ratY;

	//dispPnlAspectRatioGet(&ratX, &ratY);
	
	exifRotation = decImage->rotMode & 0x4;
	realRotation = (decImage->srcWidth < decImage->srcHeight ? 1:0);

	#if 0
	/* 代码中保持4:3比例,通过display去fit */
	if((decImage->srcWidth *ratX != ratY*decImage->srcHeight)&&(decImage->srcWidth > decImage->srcHeight)){
		decImage->fit = 0;
	}
	#endif
	
	decImage->srcWidth = (decImage->srcWidth + 15) &~15;
	
	/* get decode opt parameter */
	if (decImage->dataType == K_File_DataType_YUV420) {	
		pbLogPrintf("420!\n");	
		decImage->opt = 0x05;
		if((decImage->srcWidth %16) || (decImage->srcWidth%16)){
			//printf("file:%u,line:%u\n!\n",__FILE__,__LINE__);
			return FAIL;
		}
	} else if (decImage->dataType == K_File_DataType_YUV422) {	
		pbLogPrintf("422!\n");
		decImage->opt = 0x04;
	} else {
		printf("YUV err\n");
		return FAIL;
	}
	/* is standard huffman table */
	if(!isStdHufTab){
		printf("file:%u,line:%u\n",__FILE__,__LINE__);
		return FAIL;
	}
	
    /*get JPG decoder parameter */
	switch ( decImage->imgeType ){
		case 2:
			pOddVlc = &exifQvVlcAtOdd;
		break;
		case 1:
			pOddVlc = &exifThbVlcAtOdd;
		break;
		default :
			pOddVlc = &exifVlcAtOdd; // init pOddVlc by default
		break;
	}

	if (*pOddVlc) { // check if decode vlc data is on odd address
		*pOddVlc = 0;
		 decImage->opt |= 0x10;
	}
	
	if(pbCheckAbort( decImage->imgeType,__LINE__)){	
		return PBABORT;
	}
	
	/* get decode width and height parameter */
	if(exifRotation) { 
		decWidth = decImage->dstHeight;
		decHeight =decImage->dstWidth;		 

		decImage->decOriHeight = (UINT16)((UINT32)decImage->srcHeight * decWidth / (UINT32)decImage->srcWidth); //temp height if full widht
		decImage->decOriWidth= (UINT16)((UINT32)decImage->srcWidth * decHeight / (UINT32)decImage->srcHeight); //temp width
		pbLogPrintf("tmp: %u, %u\n", decImage.decOriWidth, decImage.decOriHeight);
		
		if(decImage->decOriWidth > decWidth){
			decImage->decOriWidth = decWidth;
		}
		else{
			decImage->decOriHeight = decHeight;
		}

		decWidth = (decImage->decOriWidth + 7) &  ~7;			
		decHeight= (decImage->decOriHeight + 7) &  ~7;	//needed by rot
	}
	else if (decImage->rotMode){
		decWidth = decImage->dstWidth;
		decHeight = (decImage->dstHeight + 1) & ~1; //decHeight = (imgH + 7) & ~7;
	}
	else{
		decWidth = decImage->dstWidth;
		decHeight = decImage->dstHeight;
	}

	/* resolve real rotation,thumbnail distortion ... */
	if( realRotation && fillBlack ){ 
		decImage->decOriWidth = (UINT16)((UINT32)decImage->srcWidth * decHeight / (UINT32)decImage->srcHeight);
		decImage->decOriHeight = (UINT16)((UINT32)decImage->srcHeight * decWidth / (UINT32)decImage->srcWidth);
		decImage->decOriWidth = decImage->decOriWidth &~7;
		if(decImage->decOriHeight > decHeight){
			decImage->decOriHeight = decHeight;
		}
		decWidth = decImage->decOriWidth;
		decHeight = decImage->decOriWidth*decImage->srcHeight / decImage->srcWidth; //keep 4:5 
	}

	pbLogPrintf("src : %u, %u\n", decImage->srcWidth, decImage->srcHeight);
	pbLogPrintf("dec : %u, %u\n", decWidth, decHeight);
	pbLogPrintf("img : %u, %u\n", decImage->dstWidth, decImage->dstHeight);

	/*get decode fac */
	if(fillBlack){
		decImage->fac = 0x08;    
		if (decImage->srcWidth > decWidth) {
			while (((decImage->srcWidth >> 3) * --decImage->fac) > decWidth);
			if(decImage->fac== 0) {
				decImage->fac = 1;
			}
		} 
		else{
			decImage->fac = 0;
		}
	}
	else{
		decBuf= decImage->srcAddr -K_SDRAM_ImagePlaybackBufAddr;
		decImage->fac =  pbDecodeFactorGet(decBuf,decImage->srcWidth,decImage->srcHeight,decImage->dstWidth,decImage->dstHeight,exifRotation);
		if(decImage->fac == 0xFF){
			return FAIL;
		}
	}
	
	/* get decode dmyX & dmyY */
	do {
		if (decImage->fac){
			decImage->dmyX = (((decImage->srcWidth * (UINT16)decImage->fac) >> 3 ) & 7);
			if (decImage->dmyX) {
				decImage->dmyX = 8 - decImage->dmyX;
			}
			if( exifRotation ){ //exchange x, y need w,h 8 multiple
				decImage->dmyY = (((decImage->srcHeight *(UINT16)decImage->fac) >> 3) & 7);
				if (decImage->dmyY) {
					decImage->dmyY = 8 - decImage->dmyY;
				}
            	decImage->decOriHeight = (((decImage->srcHeight * (UINT16)decImage->fac) >> 3) + 7) &~7;
			}
			else{
				decImage->dmyY = (((decImage->srcHeight * (UINT16)decImage->fac) >> 3) & 1);
				if (decImage->dmyY) {
					decImage->dmyY = 2 - decImage->dmyY;
				}
				decImage->decOriHeight = (((decImage->srcHeight * (UINT16)decImage->fac) >> 3) + 1) &~1;
			}
			decImage->scaleHeight= ((decImage->srcHeight * (UINT16)decImage->fac) >> 3) /*+ dmyY*/; //tq.zhang
			decImage->decOriWidth = (((decImage->srcWidth * (UINT16)decImage->fac) >> 3) + 7) &~7;
		}
		else {
			if (decImage->srcWidth & 7) {
				decImage->dmyX = 8 - (decImage->srcWidth & 7);
			}
			if(exifRotation){
				if (decImage->srcHeight& 7) {
					decImage->dmyY = 8 - (decImage->srcHeight& 7);
				}
				decImage->decOriHeight = (decImage->srcHeight + 7) &~7;
			}
			else if(decImage->srcHeight & 1){
				decImage->dmyY = 2 - (decImage->srcHeight & 1);
				decImage->decOriHeight = (decImage->srcHeight + 1) &~1;
			}
			else{
				decImage->decOriHeight = decImage->srcHeight;
			}
			decImage->decOriWidth = (decImage->srcWidth  + 7) &~7;
			decImage->scaleHeight = decImage->srcHeight;  //tq.zhang
		}
		//printf("decAddr=%lx,w=%u,h=%u\n",decImage->decAddr,decImage->decOriWidth,decImage->decOriHeight);
		if(realRotation ||decImage->dmyX ||decImage->dmyY){	
			endAddr = decImage->decAddr + (UINT32)decImage->decOriWidth*decImage->decOriHeight*2+ 32768;
			//printf("srcAddr=%lx,end=%lx\n", decImage->srcAddr,endAddr);
		}
		else{
			endAddr = decImage->decAddr + (UINT32)decImage->decOriWidth*decImage->decOriHeight + 32768; 
		}		   
		pbLogPrintf("buf: 0x%lx, 0x%lx, 0x%lx\n", decImage.->decAddr, endAddr, decImage->srcAddr);

		if(endAddr > decImage->srcAddr) {
			if (decImage->fac>1){
				decImage->fac--;
			}
			else{
				printf("L:%u\n",__LINE__);
				//printf("L%u,Addr=%lx,srcAddr=%lx\n", (UINT16)__LINE__,endAddr,decImage->srcAddr);
				return FAIL;
			}
		}
	}while((decImage->fac>0) && (endAddr >decImage->srcAddr));

	pbLogPrintf("Fac=%bu\n", decImage->fac);

	return SUCCESS;
}

/**
 * @fn		  UINT8 pbImgDeCmpRot(Exif_Decompress_Parameter* ImagePara, Pb_Dec_Parameter* DecPara, UINT8 fillBlack)
 * @brief	  still image decompress with rotation
 * @param	  [in] ImagePara : source image information
 * @param	  [in] DecPara : destination decode information
 * @param	  [in] fillBlack : keep required size and fill black if fillBlack is 1
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-6
 * @todo	  N/A
 * @bug 	  N/A
*/
#if (!STILL_LIB_PB_DMY)
UINT8 pbImgDeCmpRot(ExifImage_t* imagePara, Pb_Dec_Parameter* decPara, UINT8 fillBlack) USING_0
{
	UINT8 ret = SUCCESS;
	UINT16 MCURstNo = 0;
	UINT16 curW,curH;
	UINT32 decAddr,centAddr, sclAddr, tmpAddr;
	UINT32 dramEndAddr = K_SDRAM_TotalSize - 256UL;
	UINT32 decBuff;
	//UINT8 exifRotation,realRotation;
	UINT8 chgXY;
	pbImageDecPara_t decImage;
//	UINT32 tmp1,tmp2,dstAddr;
	UINT16 decWidth,decHeight;
//	UINT16 sclW,sclH;
	UINT32 allocBuf;
	
	decImage.srcAddr = imagePara->addr;
	decImage.dstWidth = decPara->DecW;
	decImage.dstHeight = decPara->DecH;
	decImage.rotMode = decPara->RotMode;
	decImage.srcWidth = imagePara->width;
	decImage.srcHeight = imagePara->height;
	decImage.dataType = imagePara->dataType; 
	decImage.imgeType = decPara->ImgType;	 //Main QV Thumbnail
	decImage.decAddr = decPara->DecBuff;
	decImage.dmyX = 0;
	decImage.dmyY = 0;
	#if 0
	decImage.fit = 1;
	#endif
	decBuff = decPara->DecBuff;
	centAddr = decBuff;
	//exifRotation = decImage.rotMode & 0x4;
	//realRotation = (decImage.srcWidth < decImage.srcHeight ? 1:0);
	curW = decImage.dstWidth;
	curH = decImage.dstHeight;
	
	if(!fillBlack){// fit cur size
		chgXY = (decImage.srcWidth < decImage.srcHeight) ? 1 : 0; // dstSize default is 4:3 or 16:9
		if(decImage.rotMode & 0x4){
			chgXY = !chgXY;
		}		
		if(chgXY){
			curW = decImage.dstHeight;
			curH= decImage.dstWidth;
		}
	}
	
	ret = pbImgDecParaGet( &decImage,fillBlack);
	if( ret!= SUCCESS ){
		return ret;
	}
	
	decWidth = decImage.decOriWidth;		//xian : decOriWidth is orientation width
	decHeight = decImage.decOriHeight;
	decPara->DecW = decImage.decOriWidth;
	decPara->DecH = decImage.decOriHeight;

	/* need move cent */
	if(decImage.dmyX || decImage.dmyY) {
		decAddr = centAddr + (UINT32 )decImage.decOriWidth* decImage.decOriHeight;
		if((decAddr +(UINT32)decImage.decOriWidth*decImage.decOriHeight) > dramEndAddr){
			return FAIL;
		}
		gprmBlackFill(decBuff, decImage.decOriWidth, decImage.decOriHeight);
	}
	else{
		decAddr = centAddr;
	}		

	//printf("src w=%u, h=%u,fac=%bu \n", decImage.srcWidth, decImage.srcHeight,decImage.fac);
	ret = HAL_JpegDecode(decImage.srcAddr, decImage.srcWidth, decImage.srcHeight, decAddr, decImage.fac, decImage.opt, G_JpgRstMcuNum);
	if(ret != SUCCESS ){//error happen
		return FAIL;//tmp
	}

	decImage.srcWidth = decImage.decOriWidth;
	decImage.srcHeight = decImage.scaleHeight;	 //keep the vsize
	
	pbLogPrintf("destination w is %u, h is %u \n", decWidth, decHeight);
	pbLogPrintf("dummy X is %u, Y is %u \n", decImage.dmyX, decImage.dmyY);
	pbLogPrintf("move center 0x%lx -> 0x%lx\n", decAddr, centAddr);

	if( decImage.dmyX || decImage.dmyY ){	
		if(decImage.dmyX){
			HAL_GprmCopy(
				decAddr,
				decImage.srcWidth, decImage.srcHeight, 0, 0,
				centAddr,
				decImage.srcWidth, decImage.srcHeight, ((decImage.dmyX + 1 >> 1) & ~0x01), ((decImage.dmyY + 1>> 1) & ~0x01),  
				decImage.srcWidth - decImage.dmyX, decImage.srcHeight ,//decImage.srcHeight - decImage.dmyY,
				0,
				1
			);
		}
		else{  // mantis #41854 
			tmpAddr = centAddr + (UINT32 )decImage.dstWidth * decImage.dstHeight;
			HAL_GprmScale(
				decAddr,
				decImage.srcWidth, decImage.srcHeight,
				centAddr,
				decImage.decOriWidth, decImage.decOriHeight,
				tmpAddr,
				1
			);

			decImage.dmyY = 0; //xian ++ 20121010, dmy no use if scale
		}
	}
	
	if(pbCheckAbort(decImage.imgeType,__LINE__)){	
		return PBABORT;
	}

	/* rotation -> decAddr(decWidth, decHeight) */
	if(decImage.rotMode){
		decAddr = decBuff + (UINT32)decImage.decOriWidth * decImage.decOriHeight; //decImage.decOriWidth and decImage.dstWidth are diff. xian 20110219 #42010
		pbLogPrintf("dst w=%u,h=%u,ori w=%u,h=%u\n",decImage.dstWidth,decImage.dstHeight,decImage.decOriWidth,decImage.decOriHeight);
		pbLogPrintf("rot %bu 0x%lx -> 0x%lx\n", decImage.rotMode, decBuff, decAddr);
		
		if(decAddr + (UINT32)decImage.decOriWidth * decImage.decOriHeight > dramEndAddr) {
			printf("L%u\n", (UINT16)__LINE__);
			return FAIL;
		}
		
		HAL_GprmRot(
			decImage.rotMode, 
			centAddr,
			decAddr, 
			decImage.decOriWidth, decImage.decOriHeight, 
			1
		);
		if( decImage.rotMode&0x4 ){
			//decImage.dmyX = decImage.decOriWidth; 
			decWidth = decImage.decOriHeight;
			decHeight = decImage.decOriWidth; //decImage.dmyX;
		}
		else if (decImage.rotMode){
			decWidth = decImage.decOriWidth;
			decHeight = decImage.decOriHeight;
		}
		
		pbLogPrintf("%u, %u, %u, %u\n", decImage.dstWidth, decImage.dstHeight, decWidth, decHeight);
		if(pbCheckAbort(decImage.imgeType,__LINE__)){
			return PBABORT;
		}
		
		HAL_GprmCopy(
			decAddr,
			decWidth, decHeight, 0, 0,
			decBuff,
			decWidth, decHeight, 0, 0,	
			decWidth, decHeight,
			0,
			1
		);
		
		decPara->DecW =decWidth;    //xian : real width in buffer, may be exchanged with decOriHeight
		decPara->DecH = decHeight;
	}

	allocBuf = (UINT32)decWidth * decHeight > (UINT32)curW*curH ? (UINT32)decWidth * decHeight :(UINT32)curW*curH;
	sclAddr = (decBuff + allocBuf)& ~15;
	tmpAddr = sclAddr + (UINT32)decImage.dstWidth * decImage.dstHeight;
	if((tmpAddr+(UINT32)(decWidth>curW?decWidth:curW)*(decHeight>curH?decHeight:curH))>dramEndAddr){
		printf("L:%u\n",__LINE__);
		return FAIL;
	}
	
	if(!decImage.imgeType && !fillBlack){
		HAL_GprmScale(
			decBuff,decWidth, decHeight,
			sclAddr,curW,curH,
			tmpAddr,
			1
		);
		HAL_GprmDramDma(sclAddr<<1, decBuff<<1, (UINT32)curW*curH*2, 1);
	
		decImage.dmyX = 0; //xian ++ 20121010, dmy no use if scale
		decImage.dmyY = 0;
	}
		
	/*for multi play...*/
	if( fillBlack ){// image at decBuff, decWidth, decHeight
		decPara->DecW = curW;
		decPara->DecH = curH;
		decImage.dmyX = 0;
		decImage.dmyY = 0;	
		
		if(decWidth != curW || decHeight != curH){						
			UINT16 sclW, sclH;
			sclW = curW;
			sclH = curH;
			if((UINT32)curW*decHeight < (UINT32)curH*decWidth){
				sclH = ((UINT32)curW * decHeight / decWidth) & ~1;
			}
			else {
				sclW = ((UINT32)curH * decWidth / decHeight) & ~7;
			}
			
			HAL_GprmScale(
				decBuff,decWidth, decHeight,
				sclAddr,sclW,sclH,
				tmpAddr,
				1
			);
			
			decImage.dmyX = ((curW - sclW) >> 1) & ~1;
			decImage.dmyY = ((curH - sclH) >> 1) & ~1;
			
			gprmBlackFill(decBuff, curW, curH);

			HAL_GprmCopy(
				sclAddr,
				sclW, sclH, 0, 0,
				decBuff,
				curW, curH, decImage.dmyX, decImage.dmyY,  
				sclW, sclH,
				0,
				1
			);
		}
		#if 0 //xian --		
		if(exifRotation || realRotation){  
			if(decImage.srcWidth < decImage.srcHeight){
				decWidth =decImage.decOriWidth;
				decHeight = decImage.decOriHeight;
			}
			decImage.dmyX = ((decImage.dstWidth - decWidth) >> 1) & ~1;
			decImage.dmyY = ((decImage.dstHeight - decHeight) >> 1) & ~1;	  
			pbLogPrintf("dmy: %u, %u\n", decImage.dmyX, decImage.dmyY);
			#if PB_REAL_ROTATION_SUPPORT		
			if(realRotation){
				tmpAddr = decBuff + (UINT32 )decImage.dstWidth * (UINT32)decImage.dstHeight;  
				HAL_GprmCopy(decAddr, decWidth, decHeight, 0, 0, tmpAddr, decWidth, decHeight, 0, 0,decWidth, decHeight, 0, 1);
				decAddr = tmpAddr;
			}
			#endif
			gprmBlackFill(decBuff, decImage.dstWidth, decImage.dstHeight);
			if(pbCheckAbort(decImage.imgeType,__LINE__)){
				return PBABORT;
			}
			
			HAL_GprmCopy(
				decAddr,
				decWidth, decHeight, 0, 0,
				decBuff,
				decImage.dstWidth, decImage.dstHeight, decImage.dmyX, decImage.dmyY,  
				decWidth, decHeight,
				0,
				1
			);
		}
		else{
			//scale -> sclAddr(decWidth, decHeight)
			tmpAddr = decBuff + (UINT32 )decImage.dstWidth * (UINT32)decImage.dstHeight;  //80*64;
			sclAddr = dramEndAddr - (UINT32 )decImage.dstWidth * (UINT32)decImage.dstHeight;  //80*64;
			sclAddr = sclAddr & ~15;
			
			pbLogPrintf("scale 0x%lx -> 0x%lx\n", centAddr, sclAddr);	
			if(tmpAddr + (UINT32)decImage.dstWidth *decImage.dstHeight > sclAddr) {
				pbLogPrintf("L%u\n", (UINT16)__LINE__);
				return FAIL;
			}
			
			HAL_GprmScale(
				decBuff,
				decWidth,	decHeight,
				sclAddr,
				decImage.dstWidth,decImage.dstHeight,
				tmpAddr,
				1
			);
			pbLogPrintf("srcW=%u,srcH=%u,decImage.dstWidth=%u,decImage.dstHeight=%u\n",decImage.srcWidth,decImage.srcHeight,decImage.dstWidth,decImage.dstHeight);
			HAL_GprmCopy(
				sclAddr,
				decImage.dstWidth,decImage.dstHeight,0,0,
				decBuff,
				decImage.dstWidth,decImage.dstHeight,0,0,  
				decImage.dstWidth,decImage.dstHeight,
				0,
				1
			);
			
			decImage.dmyX = 0; //xian ++ 20121010, dmy no use if scale
			decImage.dmyY = 0;
		}
		decPara->DecW = decImage.dstWidth;
		decPara->DecH = decImage.dstHeight;

		curW = decImage.dstWidth;
		curH = decImage.dstHeight;
		#endif
	}
	
	//wang ++ 20081016
	g_pb_rec_RotOut.hOff=decImage.dmyX>>1;
	g_pb_rec_RotOut.vOff=decImage.dmyY>>1;
	g_pb_rec_RotOut.hSize=curW - decImage.dmyX;
	g_pb_rec_RotOut.vSize=curH - decImage.dmyY;

	pbDecodeSizeSet(curW, curH);
	
	return SUCCESS;
}


//xian ++ 20080926
/**
 * @fn		  UINT8 pbVideoFrameDecode(UINT16 FileIndex, UINT16 decWidth, UINT16 decHeight, UINT32 freeBufAddr)
 * @brief	  decode video first frame
 * @param	  [in] FileIndex : file index
 * @param	  [in] decWidth : destination decode width
 * @param	  [in] decHeight : destination decode height
 * @param	  [in] freeBufAddr : free buffer for video decode
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-6
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 pbVideoFrameDecode(UINT16 FileIndex, UINT16 decWidth, UINT16 decHeight, UINT32 freeBufAddrWord, UINT8 fillBlack)
{
	UINT16 i;
	UINT32 backAddr = freeBufAddrWord;
	UINT32 dramEndAddr = K_SDRAM_TotalSize - 256UL;
	UINT32 vlcAddr;
	UINT8 sts ;
	UINT16 fHandle;
	UINT8 QTable[128];
	UINT8 tmp2, tmp3,tmp4,tmp5;
 
	UINT16 VideoWidth;
	UINT16 VideoHeight;

	UINT32 AviHdrSize;
	UINT32 scale,rate,frame;
	UINT32 frameSize;
	UINT8 decOpt = 0x04;
	PBFile_t* pFileInfo = &MediaInfo;

	UINT16 dmyX = 0;
	UINT16 dmyY = 0;
	UINT16 yuvType;
	//UINT32 dispAddr;
	UINT32 addr;
	sts = FALSE;


	//For Multi-Frame Play Back
	if ((pbFileInfoGet(FileIndex, pFileInfo) != SUCCESS) /*|| (FileInfo.FileType != K_PLAYBACK_TYPE_JPG)*/) {
		DBG_pbFuncTrack();
		return FAIL;
	}

	//For Multi-Frame Play Back
	fHandle = vfsFileOpen(pFileInfo->FileName, K_DOS_ReadFile, pFileInfo->FileStartCluster);
	//printf("name:%s\n",FileInfo.FileName );

	if (!fHandle) {
		// printf("Cannot open AVI file\n");
		vfsFileClose(fHandle, pFileInfo->FileName, K_DOS_ReadFile, 0);
		return FAIL;
	}

	sts = vfsFileRead(fHandle, K_DOS_DstDram, 512, backAddr, 0);

	if (!sts) {
		// DbgPrint("Cannot read AVI header\n");
		vfsFileClose(fHandle, pFileInfo->FileName, K_DOS_ReadFile, 0);
		return FAIL;
	}

	for(	i=0; i< 512/4;i++) {
		HAL_DramStartAddrSet(backAddr + (i*4)/2 , 1);
		HAL_DramWordDataRead(&tmp2, &tmp3);
		HAL_DramWordDataRead(&tmp4, &tmp5);
		if(((tmp2 == 'A')&&(tmp3 == 'V')&&(tmp4 == 'I')&&(tmp5 == 0x20))) {
			sts = SUCCESS;
			pbDramDWordGet(backAddr, (UINT32)((i+2)<<1), &AviHdrSize, 1, 1);
			AviHdrSize += 20;
			break;
		} 
	}
	if(sts!=SUCCESS){
		sts = FAIL;
		vfsFileClose(fHandle, pFileInfo->FileName, K_DOS_ReadFile, 0);
		return sts;
	}
	sts = FAIL;

	for(	i=0; i< 512/4;i++) {
		HAL_DramStartAddrSet(backAddr + (i*4)/2 , 1);
		HAL_DramWordDataRead(&tmp2, &tmp3);
		HAL_DramWordDataRead(&tmp4, &tmp5);
		if(((tmp2 == 'v')&&(tmp3 == 'i')&&(tmp4 == 'd')&&(tmp5 == 's'))) {
			sts = SUCCESS;
			i+=5;
			pbDramDWordGet(backAddr, (UINT32)((i)<<1), &scale, 1, 1);
			i+=1;
			pbDramDWordGet(backAddr, (UINT32)((i)<<1), &rate, 1, 1);
			i+=2;
			pbDramDWordGet(backAddr, (UINT32)((i)<<1), &frame, 1, 1);
			rate = rate/scale;
			rate = 1000/rate;//msperFrame
			frame = frame * rate;//total time
			aviFileLength = frame;
			break;
		}
	} 
	if(sts!=SUCCESS){
		sts = FAIL;
		vfsFileClose(fHandle, pFileInfo->FileName, K_DOS_ReadFile, 0);
		return sts;
	}
	sts = FAIL;
	for(	i=0; i< 512/4;i++) {
		HAL_DramStartAddrSet(backAddr + (i*4)/2 , 1);
		HAL_DramWordDataRead(&tmp2, &tmp3);
		HAL_DramWordDataRead(&tmp4, &tmp5);
		if(((tmp2 == 'a')&&(tmp3 == 'v')&&(tmp4 == 'i')&&(tmp5 == 'h'))) {
			UINT32 tmpW,tmpH;
			sts = SUCCESS;
			pbDramDWordGet(backAddr, (UINT32)((i+10)<<1), &tmpW, 1, 1);
			pbDramDWordGet(backAddr, (UINT32)((i+10)<<1), &tmpH, 0, 1);
			VideoWidth = (UINT16)tmpW;
			VideoHeight = (UINT16)tmpH;
			/*#if  JPG_1080P
			if(VideoHeight==1080){
				VideoHeight=1088;
			}
			#endif*/
			//printf("VidWidth:%u\n",VideoWidth);
			//printf("VidHeight:%u\n",VideoHeight);
			break;
		} 
	} 
	
#if (VIDEO_FRAME_DEBUG_LEVEL>0)
		if (1){
		UINT32 TotalFrame,DropFrame,VlcDropFrame,KBPerSecSpeed,MaxQ,MinQ,DumyIdx,Drop12Num;
		//vfsFileRead(fHandle, K_DOS_DstDram, 1024, backAddr, 0);
		for(i=78; i< 512/4;i++) {
				HAL_DramStartAddrSet(backAddr + (i*4)/2 , 1);
				HAL_DramWordDataRead(&tmp2, &tmp3);
				HAL_DramWordDataRead(&tmp4, &tmp5);
				//printf("%bx, %bx, %bx, %bx\n",tmp2,tmp3,tmp4,tmp5);
				if(((tmp2 == 'J')&&(tmp3 == 'U')&&(tmp4 == 'N')&&(tmp5 == 'K'))) {
					sts = SUCCESS;
					i+=2;
					pbDramDWordGet(backAddr, (UINT32)((i)<<1), &TotalFrame, 1, 1);
					//i+=1;
					pbDramDWordGet(backAddr, (UINT32)((i)<<1), &DropFrame, 0, 1);
					//i+=1;
					pbDramDWordGet(backAddr, (UINT32)((i)<<1), &VlcDropFrame, 0, 1);
					//i+=1;
					pbDramDWordGet(backAddr, (UINT32)((i)<<1), &KBPerSecSpeed, 0, 1);
					//i+=1;
					pbDramDWordGet(backAddr, (UINT32)((i)<<1), &MaxQ, 0, 1);
					//i+=1;
					pbDramDWordGet(backAddr, (UINT32)((i)<<1), &MinQ, 0, 1);
					//i+=1;
					pbDramDWordGet(backAddr, (UINT32)((i)<<1), &DumyIdx, 0, 1);
					//i+=1;
					pbDramDWordGet(backAddr, (UINT32)((i)<<1), &Drop12Num, 0, 1);
					/*printf("TotalFrame:%ld\n",TotalFrame);
					printf("DropFrame:%ld\n",DropFrame);
					printf("VlcDropFrame:%ld\n",VlcDropFrame);
					printf("KBPerSec: %ldKB/s\n",KBPerSecSpeed);
					printf("Q Max:%ld\n",MaxQ);
					printf("Q Min:%ld\n",MinQ);
					printf("DumyIdx:%ld\n",DumyIdx);
					printf("Drop12Num:%ld\n",Drop12Num);*/
					break;
				}
			} 
		}
#endif
	if(sts!=SUCCESS){
		//printf("--%d__NO AVI File\n",(UINT16)__LINE__);
		sts = FAIL;
		vfsFileClose(fHandle, pFileInfo->FileName, K_DOS_ReadFile, 0);
		return sts;
	}
	sts = FAIL;
	
	sts = vfsFileRead(fHandle, K_DOS_DstDram, 1024, backAddr, (AviHdrSize>>9)<<9);

	for(i=0; i< 1024/4;i++) {
		addr = backAddr + (i*4)/2;
		HAL_DramStartAddrSet(backAddr + (i*4)/2 , 1);
		HAL_DramWordDataRead(&tmp2, &tmp3);
		HAL_DramWordDataRead(&tmp4, &tmp5);
		if(((tmp2 == 'm')&&(tmp3 == 'o')&&(tmp4 == 'v')&&(tmp5 == 'i'))) {
			sts = SUCCESS;
			i += 2;
			pbDramDWordGet(backAddr, (UINT32)((i)<<1), &frameSize, 1, 1);
			HAL_DmaSetSramAddr(0x0000);
			HAL_DmaSetDramAddr(addr -4);
			backAddr = addr-4;
			HAL_DmaDo(0, 1, 512, 0, 0);

			//check QDT (FF DB 00 C5)
			i = 0x28;
            tmp2 = XBYTE[0x3800 + i]; 
            tmp3 = XBYTE[0x3800 + i + 1];
            tmp4 = XBYTE[0x3800 + i + 2];
            tmp5 = XBYTE[0x3800 + i + 3];
			
			for (i = 0; i < 64; i++) {
				QTable[i] = XBYTE[0x382D + i];
			}

			for (i = 64; i < 128; i++) {
				QTable[i] = XBYTE[0x382E + i];
			}

			jpegQTableSet( 0, QTable, QTable + 64, 1 );

			break;
		}
	 }
	if(sts!=SUCCESS){
//		printf("--%d__NO AVI File\n",(UINT16)__LINE__);
		sts = FAIL;
		vfsFileClose(fHandle, pFileInfo->FileName, K_DOS_ReadFile, 0);
		return sts;
	}
	
	//printf("size=0x%lx, 0x%lx\n", frameSize, AviHdrSize);
	//+20: LIST....movi....
	frameSize = (((frameSize+20)>>9)+1)<<9;
	vlcAddr = backAddr;
	sts = vfsFileRead(fHandle, K_DOS_DstDram, ((frameSize+(AviHdrSize-((AviHdrSize>>9)<<9)))+511)&~511, backAddr, (AviHdrSize>>9)<<9 );	
	//sts = vfsFileRead(fHandle, K_DOS_DstDram, (((frameSize+20)>>9)+1)<<9, backAddr, (AviHdrSize>>9)<<9 );
	for(i=0; i<680;i++) {
		//HAL_DramStartAddrSet(backAddr + (i*4)/2 , 1);
		HAL_DramStartAddrSet(vlcAddr, 1);
		HAL_DramWordDataRead(&tmp2, &tmp3);
		HAL_DramWordDataRead(&tmp4, &tmp5);
		vlcAddr += 2;
		if(((tmp2 == 0x11)&&(tmp3 ==0x00)&&(tmp4 == 0x3f)&&(tmp5 ==0x00))) {
			sts = SUCCESS;
			break;
		}
	} 
	vfsFileClose(fHandle, pFileInfo->FileName, K_DOS_ReadFile, 0);
	if(sts!=SUCCESS){
//		printf("--%d__NO AVI File\n",(UINT16)__LINE__);
		sts = FAIL;
		return sts;
	}
	
	//vlcAddr = backAddr+ (i+1)*4/2;
	yuvType=videoPbReadDword(vlcAddr-(UINT32)11);
	yuvType &= 0x00ff;
	if(VideoWidth==1920){
		if(yuvType==0x0022 ){
			if(VideoHeight%16!=0){
				VideoHeight+=8;
				}
			}
	}
	pFileInfo->MediaWidth = VideoWidth;
	pFileInfo->MediaHeight= VideoHeight;	
#if 0 //xian -- 20110214 , do this before pbVideoFrameDecode()
	HAL_PbBufAddrGet(&dispAddr);
	if(osSysModeGet()== OS_SYS_MODE_STILL_PB &&dispDevActGet() ==4 && ((VideoWidth==1280 && VideoHeight==720)||(VideoWidth==848 &&VideoHeight==480) )&& (decWidth>160) && (decHeight>120) ){
		if ((VideoWidth==1280 && VideoHeight==720)){
			decWidth = 960;
			decHeight = 540;
		}else{
			decWidth = VideoWidth;
			decHeight = VideoHeight;
		}
#if 0 //xian --
		if (dispAddr != pbDispBuffGet(1)) {
			HAL_PbBufAddrSet(pbDispBuffGet(1));

			HAL_PbBufHsizeSet(decWidth);
			HAL_DispScaleSet(decWidth, decHeight, 100,0);
			HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PB);
			HAL_DramZUpdateWait(HAL_DRAM_ZUPDATE_PB, 0);
		}
#else		
		if(pbDispBufModeGet(NULL,NULL) == PB_DISP_BUF_MODE_ZOOM){
			//printf("3^^ %lx\n", dispAddr);
			dispShow(pbDispBuffGet(1), decWidth, decHeight, 100);
		}
#endif
	}
#endif

	return pbJpgDecodeSizeFit(vlcAddr, frameSize, VideoWidth, VideoHeight, freeBufAddrWord, decWidth, decHeight, fillBlack);
}

//condition: imgW%16==0,
/**
 * @fn		  UINT8 pbJpgDecodeWidthFitFactGet(UINT16 srcW, UINT16 dstW)
 * @brief	  get decode factor
 * @param	  [in] srcW
 * @param	  [in] dstW
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-6
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 pbJpgDecodeWidthFitFactGet(UINT16 srcW, UINT16 dstW)
{
	UINT8 fact, fit = 0;
	UINT16 tmpW;
	
	dstW <<= 3; // w*8
	fact = dstW / srcW; 	//nearest
	if(!fact){
		fact = 1;			 // min fact
	}
	//find best fact, no need scale
	if(fact > 8){
		fact = 8;	   // max fact
		fit = 1;
	}	 
	else if((tmpW=srcW*fact) == dstW){
		fit = 1;
	}
	else if( (tmpW+=srcW) == dstW){
		fact ++;
		fit = 1;
	}	 
	//find fact(2, 4, 6), no need fill black
	if(!fit){
		fact = (fact + 1) & ~1;    
		if(srcW*fact & 0x3f)
			fact = 4;
	}
	fact &= 7;
	if(fact == 6){
		fact=4;
	}
	pbLogPrintf("fact:%bu\n", fact);
	return fact; 
}

/**
 * @fn		  UINT8 pbJpgDecodeSizeFit(UINT32 vlcAddr, UINT32 vlcSize, UINT16 imgW, UINT16 imgH, UINT32 dstAddr, UINT16 dstW, UINT16 dstH)
 * @brief	  pbJpgDecodeSizeFit
 * @param	  [in] vlcAddr
 * @param	  [in] vlcSize
 * @param	  [in] imgW
 * @param	  [in] imgH
 * @param	  [in] dstAddr
 * @param	  [in] dstW
 * @param	  [in] dstH
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-6
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 
pbJpgDecodeSizeFit(
	UINT32 vlcAddr,
	UINT32 vlcSize,
	UINT16 imgW,
	UINT16 imgH,
	UINT32 dstAddr,
	UINT16 dstW,
	UINT16 dstH,
	UINT8 fillBlack
)
{
	UINT8 fact;
	UINT16 decW, decH, sclW, sclH;
	UINT32 tmp1, tmp2;
	UINT32 decAddr, sclAddr, tmpAddr;
	UINT32 tmpSize;
	UINT32 dramEndAddr = K_SDRAM_TotalSize - 256UL;
	UINT16 dramread;// tq.zhang video yuv420
	UINT8 opt=0x04; // tq.zhang video yuv420

	tmp1 = (UINT32)imgW * dstH;
	tmp2 = (UINT32)dstW * imgH;
	if(tmp1 >= tmp2){//fit width
		decW = dstW;		
	}
	else{
		decW = tmp1 / imgH; //fill black left and right 		   
	}
	fact = pbJpgDecodeWidthFitFactGet(imgW, decW);
	if(fact){
		decW = (((imgW * fact) >> 3) + 7) &~7;
		decH = ((imgH * fact) >> 3);
	}
	else{
		decW = imgW;
		decH = imgH;
	}
	//printf("dec: %u, %u\n", decW, decH);
	//printf("dst: %u, %u\n", dstW, dstH);
	decAddr = dramEndAddr - (UINT32)decW * decH;
	if(vlcAddr + vlcSize > decAddr){
		pbLogPrintf("out of mem\n");
		return FAIL;
	}

	if (1){// tq.zhang video yuv420
		dramread=videoPbReadDword(vlcAddr-(UINT32)11);
		dramread &= 0x00ff;
		if (dramread==0x0021){
			opt &= 0xfc;
			opt |= 0x00;
		}
		else if (dramread==0x0022){
			opt &= 0xfc;
			opt |= 0x01;
		}
	} 

	if(!fillBlack){ //xian ++ 20101228
		dstW = decW;
		dstH = decH;
	}	
	
	sclW = dstW;
	sclH = dstH;
	//SY add for HD play on HDMI s
	if(osSysModeGet()== OS_SYS_MODE_STILL_PB&& (dispDevActGet() == 4 || dispDevActGet() == 2) && ((imgW==1280 && imgH==720)||(imgW==720 &&imgH==480) )&& (dstW>160) && (dstH>120)){//modify for D1 size
		HAL_JpegDecode(vlcAddr, imgW, imgH, decAddr, fact, opt, 0x00);
		#if 1
		HAL_GprmCopy(
			decAddr,
			decW, decH, 0, 0,
			dstAddr,
			decW, decH, 0, 0,
			decW, decH,
			0,
			1
		);
		#endif
		//xian -- 20110214
		//decW_HDMI = decW;
		//decH_HDMI = decH;
		g_pb_rec_RotOut.hOff = (0)>>1;
		g_pb_rec_RotOut.vOff = (0)>>1;
		g_pb_rec_RotOut.hSize = decW;
		g_pb_rec_RotOut.vSize = decH;		

		pbDecodeW = decW;
		pbDecodeH = decH;
		return SUCCESS;
	}else{
		HAL_JpegDecode(vlcAddr, imgW, imgH, decAddr, fact, opt, 0x00);
	}	
	//SY add for HD play on HDMI e
	if(decW == dstW && decH == dstH){
		HAL_GprmCopy(
			decAddr,
			decW, decH, 0, 0,
			dstAddr,
			dstW, dstH, 0, 0,
			decW, decH,
			0,
			1
		);
	}
	else{
		tmp1 = (UINT32)decW * dstH;
		tmp2 = (UINT32)dstW * decH;
		if(tmp1 >= tmp2){//fit width
			sclH = tmp2 / decW + 1;
			sclH &= ~1;
			tmpSize = (UINT32)(decW>=dstW?decW:dstW) * (decH>=sclH?decH:sclH);
			tmpAddr = dstAddr + tmpSize;
			if(sclH != dstH){
				gprmBlackFill(dstAddr, dstW, dstH);
				dstAddr += (UINT32)dstW * ((dstH - sclH)>>1);
			}
			HAL_GprmScale(
				decAddr,
				decW, decH,
				dstAddr,
				dstW, sclH,
				tmpAddr,
				1
			);		  
		}
		else{// fit height
			sclW = (tmp1 / decH)&~0x07;
			tmpSize = (UINT32)(decW>=sclW?decW:sclW) * (decH>=dstH?decH:dstH);
			sclAddr = dstAddr + (UINT32)dstW * dstH;
			tmpAddr = sclAddr + tmpSize;
			HAL_GprmScale(
				decAddr,
				decW, decH,
				sclAddr,
				sclW, dstH,
				tmpAddr,
				1
			);		  
						
			gprmBlackFill(dstAddr, dstW, dstH);
			HAL_GprmCopy(
				sclAddr,
				sclW, dstH, 0, 0,
				dstAddr,
				dstW, dstH, ((dstW-sclW)>>1)&~1, 0,
				sclW, dstH,
				0,
				1
			);
		}		
	}
	//printf("scl: %u, %u\n", sclW, sclH);
	g_pb_rec_RotOut.hOff = (dstW - sclW)>>1;
	g_pb_rec_RotOut.vOff = (dstH - sclH)>>1;
	g_pb_rec_RotOut.hSize = sclW;
	g_pb_rec_RotOut.vSize = sclH;

	pbDecodeW = decW;
	pbDecodeH = decH;
	return SUCCESS;
}
#endif

#if STILL_LIB_PB_DMY
void pbDispRoiSizeGet(UINT16 *pHOff, UINT16 *pVOff, UINT16 *pHSize, UINT16 *pVSize)
{
        pHOff=pHOff;
        pVOff=pVOff; 
        pHSize=pHSize; 
        pVSize=pVSize;
}
#else
void pbDispRoiSizeGet(UINT16 *pHOff, UINT16 *pVOff, UINT16 *pHSize, UINT16 *pVSize)
{
	if(pHOff)
		*pHOff = g_pb_rec_RotOut.hOff;	
	if(pVOff)
		*pVOff = g_pb_rec_RotOut.vOff;
	if(pHSize)
		*pHSize = g_pb_rec_RotOut.hSize;
	if(pVSize)
		*pVSize = g_pb_rec_RotOut.vSize;		
}
#endif

#if (!STILL_LIB_PB_DMY)
void pbDramDWordGet(UINT32 AddrWord, UINT32 AddrOff, UINT32* DataDWord, UINT8 RstAddrEn, UINT8 order){
	UINT8 tmp2, tmp3, tmp4, tmp5;
	if (RstAddrEn){
		HAL_DramStartAddrSet(AddrWord + AddrOff , 1);
	}
	HAL_DramWordDataRead(&tmp2, &tmp3);
	HAL_DramWordDataRead(&tmp4, &tmp5);
	if (order){
		*DataDWord = (UINT32)tmp2 | ((UINT32)tmp3<<8)| ((UINT32)tmp4<<16)| ((UINT32)tmp5<<24);
	}else{
		*DataDWord = (UINT32)tmp5 | ((UINT32)tmp4<<8)| ((UINT32)tmp3<<16)| ((UINT32)tmp2<<24);
	}
}
void pbDispBufModeSet(UINT8 mode)
{
	pbDispBufMode = mode;
}

UINT8 pbDispBufModeGet(void)
{
	return pbDispBufMode;
}

void pbDispQVSizeSet(UINT16 Width, UINT16 Height)
{
	pbDispQVW = Width;
	pbDispQVH = Height;
}

void pbDispQVSizeGet(UINT16 *Width, UINT16 *Height)
{
	*Width = pbDispQVW;
	*Height = pbDispQVH;
}

#else
void pbDispQVSizeSet(UINT16 Width, UINT16 Height)
{
	return;
	Width = Width;
	Height = Height;
}
void pbDispQVSizeGet(UINT16 *Width, UINT16 *Height)
{
	return;
	Width = Width;
	Height = Height;
}
#endif

/**
 * @fn	UINT8 pbAviMMsFun()
 * @brief	  generate thumbnail after video rec filish
 * @param	  NONE
 * @retval	  FAIL/SUCCESS
 * @see 	  NULL
 * @author	  hy.ren
 * @since	  2010-5-10
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 pbAviMMsFun(UINT8 *fileName,UINT16 width,UINT16 height)
{
	fileName = fileName;
	width = width;
	height = height;
#if 0
	UINT32 dramStartAddr = K_SDRAM_CommonEndAddr+2048;//reverse 4K buffer
	UINT32 mmsHdrSize;
	UINT8 ret;
	UINT8 thubFileName[12];
	UINT32 frameSize;
	UINT8 qTable[128];
	UINT32 thubAddr,tmpAddr,tmp1Addr;
	UINT32 thubSize;
	UINT8 i,offs;
	UINT8 opt = 0x04; //K_File_DataType_YUV422;
	UINT16 decWidth,decHeight;
	UINT32 thubYuvAddr;
	UINT8 fac;
	UINT32 aviHdrSize;
	UINT32 dramEndAddr = K_SDRAM_TotalSize - 256UL;
	UINT32 vlcAddr;
	UINT16 fHandle;
	UINT8 tmp2, tmp3,tmp4,tmp5;
	UINT16 thbWidth=160,thbHeight=120; //default value
	UINT16 fileIndx = 0;
	UINT8 ch;
	
	fHandle = vfsFileOpen(fileName, K_DOS_ReadFile, 0);
	if (!fHandle) {
		vfsFileClose(fHandle, fileName, K_DOS_ReadFile, 0);
		return FAIL;
	}
	//printf("name:%s\n",fileName);
	vfsFileRead(fHandle, K_DOS_DstDram, 512, dramStartAddr, 0);
	for(i=0; i< 512/4;i++) {
		HAL_DramStartAddrSet(dramStartAddr + (i*4)/2 , 1);
		HAL_DramWordDataRead(&tmp2, &tmp3);
		HAL_DramWordDataRead(&tmp4, &tmp5);
		if(((tmp2 == 'A')&&(tmp3 == 'V')&&(tmp4 == 'I')&&(tmp5 == 0x20))) {
			ret = SUCCESS;
			pbDramDWordGet(dramStartAddr, (UINT32)((i+2)<<1), &aviHdrSize, 1, 1);
			aviHdrSize += 20;
			break;
		} 
	}
	if(ret!=SUCCESS){
		ret = FAIL;
		vfsFileClose(fHandle, fileName, K_DOS_ReadFile, 0);
		return ret;
	}
	//printf("hdr=%lx\n",aviHdrSize);
	aviHdrSize+=(12+8);
	vfsFileRead(fHandle, K_DOS_DstDram, 1024, dramStartAddr, (aviHdrSize>>9)<<9);
	for(i=0; i<(20>>2);i++) {
		HAL_DramStartAddrSet(dramStartAddr + (i<<1), 1);
		HAL_DramWordDataRead(&tmp2, &tmp3);
		HAL_DramWordDataRead(&tmp4, &tmp5);
		if(((tmp2 == '0')&&(tmp3 == '0')&&(tmp4 == 'd')&&(tmp5 == 'c'))) {
			ret = SUCCESS;
			pbDramDWordGet(dramStartAddr + (i<<1), 2,&frameSize,1,1);
			offs = (i+2)<<2;
			break;
		} 
	}
	//pbDramDWordGet(dramStartAddr, 2,&frameSize,1,1);
	//printf("frame=%lx\n",frameSize);
	HAL_DmaSetSramAddr(0x0000);
	HAL_DmaSetDramAddr(dramStartAddr + ((offs - 8) >> 1));
	HAL_DmaDo(0, 1, 512, 0, 0);
	for (i = 0; i < 64; i++) {
		qTable[i] = XBYTE[0x3821 + i];
	}
	for (i = 64; i < 128; i++) {
		qTable[i] = XBYTE[0x3822 + i];
	}
	jpegQTableSet(0, qTable, qTable+64, 1);
	frameSize = ((frameSize>>9)+1)<<9;
	vlcAddr = dramStartAddr;
	ret = vfsFileRead(fHandle, K_DOS_DstDram, frameSize+offs, dramStartAddr, (aviHdrSize>>9)<<9 );
	for(i=0; i< 1024/4;i++) {
		HAL_DramStartAddrSet(vlcAddr, 1);
		HAL_DramWordDataRead(&tmp2, &tmp3);
		HAL_DramWordDataRead(&tmp4, &tmp5);
		vlcAddr += 2;
		if(((tmp2 == 0x11)&&(tmp3 ==0x00)&&(tmp4 == 0x3f)&&(tmp5 ==0x00))) {
			ret = SUCCESS;
			break;
		}
	} 
	vfsFileClose(fHandle, fileName, K_DOS_ReadFile, 0);
	switch(width){
		case 320:
			fac =4;
			break;
		case 720:
			fac = 4;
			thbWidth = 144;
			thbHeight = 96;
			break;
		case 1280:
			fac =4;
			thbWidth = 160;
			thbHeight = 96;
		break;
		case 640:
			fac = 2;
			break;
		case 1920:
			fac =2;
			thbWidth = 160;
			thbHeight = 112;
			opt = 0x05;
		break;
		default:
			fac  =4;
		break;
	}
	decWidth = width *fac/8;
	decHeight = height*fac/8;
	tmpAddr = vlcAddr +(frameSize>>1) + 0x100;
	
	ret = HAL_JpegDecode(vlcAddr,width ,height,tmpAddr,fac,opt,0); //640*480
	if(ret != SUCCESS ){//error happen
		printf("Jpg Err\n");
		return FAIL;
	}
	
	thubYuvAddr = tmpAddr+(UINT32)decWidth*decHeight;
	tmp1Addr = thubYuvAddr +(UINT32)decHeight*decHeight;
	
	HAL_GprmScale(
		tmpAddr,
		decWidth, decHeight,
		thubYuvAddr,
		thbWidth, thbHeight,
		tmp1Addr,
		1
	);

	/*Encode YUV422:16X8 YUV420:16X16 */
	thubAddr = thubYuvAddr +(UINT32)160*120;
	jpegQTableSet(85, NULL, NULL, 1); 
	HAL_JpgZFactorSet( 100, 100, thbWidth, thbHeight, thbWidth, thbHeight, 0);//jpg encode not support scale down function
	HAL_JpegEncode( thubYuvAddr, thubAddr, 0,0x20, 1 );
	HAL_JpegReadEncodeSize( &thubSize); 
	mmsHdrSize = exifAddMmsHdr(thubAddr,thubSize,thbWidth,thbHeight,85);

	for(i=4;i<8;i++){
		ch = (UINT8)fileName[i] - '0';
		fileIndx = fileIndx * 10;
	    fileIndx += ch;
	}
	//printf("idx=%d\n",fileIndx);
	dcfMmsFileNameGet(thubFileName,fileIndx);
	dcfEntryMmsDir();
	dramFileSave( thubFileName, thubAddr-(0x254>>1), thubSize+mmsHdrSize);
	dcfExitMmsDir();
#endif		
	return SUCCESS;
}
