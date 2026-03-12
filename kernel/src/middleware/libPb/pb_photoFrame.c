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
 * @file	  pb_photoFrame.c
 * @brief	  photoframe playback function
 * @author	  tq.zhang
 * @since	  2010-05-12
 * @date	  2010-05-12
*/

/**************************************************************************
 *						  H E A D E R	F I L E S
 **************************************************************************/
#include "general.h"
#include "pb_photoFrame.h"
#include "pb_api.h"
#include "exif_def.h" // xl
#include "Doslink.h"
#include "initio.h"
#include "pb_debug.h"
#include "cardlink.h"
#include "exiffunc.h"
#include "sp1k_snap_api.h"
#include "hal_jpeg.h"
#include "disp_api.h"
#include "util_api.h"
#include "snap_qtable.h"
#include "sp1k_rsvblk_api.h"
#include "hal_gprm.h"
#include "hal_dram.h"
#include "vfs_api.h"
#include "gprm.h"
#include "Exif_entry.h"
#include "spe_distort.h"
#include "dbg_def.h"
#include "utility.h"
#include "dbg_mem.h"
/**************************************************************************
 *							 C O N S T A N T S							  *
 **************************************************************************/
 #define __FILE	__FILE_ID_PB_PHOTOFRAME__

/**************************************************************************
 *								M A C R O S 							  *
 **************************************************************************/
#if (!STILL_PHOTOFRAME_DMY)
#define ROT_0 0x01
#define ROT_90 0x06
#define ROT_180 0x03
#define ROT_270 0x08
#define OrderToAddr(addr, offset)		((addr) = ((addr) + ((offset) - 1)) &~ ((offset) - 1))


/**************************************************************************
 *							D A T A    T Y P E S						  *
 **************************************************************************/

/**************************************************************************
 *						   G L O B A L	  D A T A						  *
 **************************************************************************/
xdata UINT8 dmyX, dmyY;
xdata UINT16 scaleChunk;
xdata UINT8 g_pbDecAlignOpt = 0; //for distortion, 0: centor, 1 : top_left

/**************************************************************************
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/
//extern xdata   UINT32  G_DOS_FileSize;
extern PBFile_t xdata MediaInfo;
//extern xdata UINT8 NeedReadMoreHeader;
extern xdata UINT8 exifAppN;
extern UINT8 xdata imgScaleFac;
extern xdata UINT8 exifVlcAtOdd;
extern xdata UINT16 G_JpgRstMcuNum;

extern RECT16 g_pb_rec_RotOut;

/**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/
UINT8 pbDecodePhotoFrameImage( UINT32 frameResIdx, UINT32 *decAddr, UINT32 endAddr, UINT16 *frameWidth, UINT16 *frameHeight );

/**
 * @fn		  UINT8 pbPhotoFrameView(UINT16 fileIndex, UINT32 frameResIdx)
 * @brief	  playback photoframe image file
 * @param	  [in] fileIndex : jpeg file index
 * @param	  [in] frameResIdx : resource photoframe file index
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-12
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 pbPhotoFrameView(
	UINT16 fileIndex,
	UINT32 frameResIdx
)
{
	UINT8 ret = SUCCESS;
	UINT32 startAddr = K_SDRAM_ImagePlaybackBufAddr;
	UINT32 frameAddr, imageAddr;
	UINT16 decWidth = PB_DECODE_WIDTH, decHeight = PB_DECODE_HEIGHT;
	UINT16 imageWidth, imageHeight;
	UINT16 frameWidth = PB_DECODE_WIDTH, frameHeight = PB_DECODE_HEIGHT;
	UINT8 decOpt = PB_DECODE_OPT_QVGA;
	UINT16 fHandle = TRUE;
	UINT32 endAddr = K_SDRAM_TotalSize-256UL;

	UINT8 rotOpt,rot=0;
	UINT32 tmpAddr;//,imgAddr;
	UINT16 hOff=0,vOff=0,hSize=PB_DECODE_WIDTH,vSize=PB_DECODE_HEIGHT;
	UINT32 yDis= (UINT32)hSize*vSize+32;
	//read frame 2 buffer
	imageAddr=startAddr;
	OrderToAddr(imageAddr, 16);
	frameAddr = imageAddr+yDis;
	OrderToAddr(frameAddr, 16);

	pbDecodePhotoFrameImage( frameResIdx, &frameAddr, endAddr, &frameWidth, & frameHeight );

	tmpAddr=frameAddr + (UINT32)frameWidth*frameHeight;
	OrderToAddr(tmpAddr, 16);
	scaleChunk = 0x20;
	ret = pbDecodeImage(
					fileIndex, &imageAddr, endAddr, decOpt,
					&imageWidth, &imageHeight,
					&decWidth, &decHeight
					);
	if( ret == FAIL )
		return ret;
	//combine
	pbPhotoFrameCombine(
						imageAddr,//pbDispBuffGet( 1 ),
						PB_DECODE_WIDTH, PB_DECODE_HEIGHT,
						frameAddr, frameWidth, frameHeight
						);
	rotOpt=pbParamGet(PB_PARAM_EXIF_ORIENT);
	//printf("rotOpt:%u\n",(UINT16)rotOpt);
	switch(rotOpt){
		case ROT_180:
			rot=3;
			break;
		case ROT_90:
			rot=5;
		break;
		case ROT_270:
			rot=6;
		break;
	}

	//printf("rot:%u\n",(UINT16)rot);
	if(rot)
	{
		HAL_GprmRot(
			rot,
			imageAddr,
			frameAddr,
			hSize,
			vSize,
			1);

	if(rot>3)
	{
		  hSize=(UINT32)PB_DECODE_HEIGHT*PB_DECODE_HEIGHT/PB_DECODE_WIDTH;
	  OrderToAddr(hSize, 8);
	  vSize=PB_DECODE_HEIGHT;
	  hOff=(PB_DECODE_WIDTH-hSize)/2;
	  vOff=0;
	  HAL_GprmScale(
					frameAddr,
					decHeight,decWidth,
					tmpAddr,
					hSize, vSize,
					imageAddr,
					1
					);
	  gprmBlackFill(imageAddr, PB_DECODE_WIDTH, PB_DECODE_HEIGHT);
	  frameAddr=tmpAddr;
	}
	//printf("size:(%u,%u),off(%u,%u)\n",hSize,vSize,hOff,vOff);

	HAL_GprmCopy(
		frameAddr,//UINT32 srcaddr,
		hSize,//UINT16 srchsize,
		vSize,//UINT16 srcvsize,
		0,//UINT16 srchoft,
		0,//UINT16 srcvoft,
		imageAddr,//UINT32 dtnaddr,
		PB_DECODE_WIDTH,//UINT16 dtnhsize,
		PB_DECODE_HEIGHT,//UINT16 dtnvsize,
		hOff,//UINT16 dtnhoft,
		vOff,//UINT16 dtnvoft,
		hSize,//UINT16 cphsize,
		vSize,//UINT16 cpvsize,
		0,//UINT16 pastethd,
		1//UINT8 wait
		);
	}
	pbDisplayImage( imageAddr, PB_DECODE_WIDTH, PB_DECODE_HEIGHT );
	return SUCCESS;
	}
#endif
/**
 * @fn		  UINT8 pbPhotoFrameSave(UINT16 fileIndex, UINT32 frameResIdx)
 * @brief	  save photoframe jpeg file
 * @param	  [in] fileIndex : jpeg index
 * @param	  [in] frameResIdx : resource photoframe index
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-12
 * @todo	  N/A
 * @bug 	  N/A
*/
#if STILL_PHOTOFRAME_DMY
UINT8 pbPhotoFrameSave(
	UINT16 fileIndex,
	UINT32 frameResIdx
) {
        fileIndex=fileIndex;
        frameResIdx=frameResIdx;
        return SUCCESS;
}
#else
UINT8 pbPhotoFrameSave(
	UINT16 fileIndex,
	UINT32 frameResIdx
) {
	UINT8 ret = SUCCESS;
	UINT32 startAddr = K_SDRAM_ImagePlaybackBufAddr;
	UINT16 decWidth = 0, decHeight = 0, imageWidth, imageHeight;
	UINT32 imageAddr, frameAddr;
	UINT16 frameWidth = 0, frameHeight = 0;
	UINT8 decOpt = PB_DECODE_OPT_FULL;
	UINT16 fHandle = TRUE;
	UINT32 endAddr = K_SDRAM_TotalSize-256UL;

	//read frame 2 buffer
	frameAddr = startAddr;
	pbDecodePhotoFrameImage( frameResIdx, &frameAddr, endAddr, &frameWidth, & frameHeight );

	scaleChunk = 0x20;
	imageAddr = frameAddr + (UINT32 )frameWidth * frameHeight;
	OrderToAddr( imageAddr, 16 );
	ret = pbDecodeImage(
					fileIndex, &imageAddr, endAddr, decOpt,
					&imageWidth, &imageHeight,
					&decWidth, &decHeight
					);
	if( ret == FAIL )
	return ret;

	//combine
	pbPhotoFrameCombine(
						imageAddr, decWidth, decHeight,
						frameAddr, frameWidth, frameHeight
						);

	//encode
	pbEncodeImage(
			imageAddr,
			decWidth, decHeight,
			//qvAddr,
			imageWidth, imageHeight
			);

	return ret;
	}
#endif
/**
 * @fn		  UINT8 pbDecodePhotoFrameImage(UINT32 frameResIdx, UINT32 *decAddr, UINT32 endAddr, UINT16 *frameWidth, UINT16 *frameHeight)
 * @brief	  decode PhotoFrame Image
 * @param	  [in] frameResIdx : resource photoframe file index
 * @param	  [in] decAddrWord : decode address
 * @param	  [in] endAddrWord : decode end address
 * @param	  [in] frameWidth : image width
 * @param	  [in] frameHeight : image height
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-12
 * @todo	  N/A
 * @bug 	  N/A
*/
#if STILL_PHOTOFRAME_DMY
UINT8 pbDecodePhotoFrameImage( UINT32 frameResIdx, UINT32 *decAddrWord, UINT32 endAddrWord, UINT16 *frameWidth, UINT16 *frameHeight )
{
        frameResIdx=frameResIdx;
        decAddrWord=decAddrWord;
        endAddrWord=endAddrWord;
        frameWidth=frameWidth;
        frameHeight =frameHeight;
        return SUCCESS;
}
#else
UINT8 pbDecodePhotoFrameImage( UINT32 frameResIdx, UINT32 *decAddrWord, UINT32 endAddrWord, UINT16 *frameWidth, UINT16 *frameHeight )
{
//	struct Exif_Decompress_Parameter mainPara;
//	struct Exif_Decompress_Parameter thumbPara;//xl
	ExifImage_t mainPara, thumbPara;
	UINT32 fileAddr, fileSize;
	UINT8 ret = FAIL, sts = TRUE;

	sp1kNandRsvSizeGet( frameResIdx, NULL, &fileSize );
	if( fileSize / 2 > endAddrWord - *decAddrWord )
		return FAIL;
	fileAddr = endAddrWord - ( fileSize >> 1 ) - 256UL;
	//printf("idAddr = %lx, idSize = %lu\n", fileAddr, fileSize);
	OrderToAddr( fileAddr, 16 );
	sp1kNandRsvRead( frameResIdx, fileAddr << 1 );

	sts = exifParserStart(exifGetMainInfo, &mainPara, fileAddr, fileSize);
	sts = exifParserStart(exifGetThumbInfo, &thumbPara, fileAddr, fileSize);

	//sts = exifParsingFile(fileAddr, fileSize, 0, NULL, &mainPara, &thumbPara);
	if( sts != TRUE )
		return FAIL;
	ret = pbDecodeImageData( &mainPara, decAddrWord, endAddrWord, frameWidth, frameHeight );
	return ret;
}
#endif
/**
 * @fn		  UINT8 pbDecodeImage(UINT16 fileIndex, UINT32 *decAddrWord, UINT32 endAddrWord, UINT8 decOpt, UINT16 *imageWidth, UINT16 *imageHeight, UINT16 *destWidth, UINT16 *destHeight)
 * @brief	  decode image with photoframe
 * @param	  [in] fileIndex : file index
 * @param	  [in] decAddrWord : decode address
 * @param	  [in] endAddrWord : end address
 * @param	  [in] decOpt : QVGA/Thumbnail/Full
 * @param	  [in] imageWidth
 * @param	  [in] imageHeight
 * @param	  [in] destWidth
 * @param	  [in] destHeight
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-12
 * @todo	  N/A
 * @bug 	  N/A
*/
#if STILL_PHOTOFRAME_DMY
UINT8 pbDecodeImage(
	UINT16 fileIndex,
	UINT32 *decAddrWord,
	UINT32 endAddrWord,
	UINT8 decOpt,
	UINT16 *imageWidth,
	UINT16 *imageHeight,
	UINT16 *destWidth,
	UINT16 *destHeight
) {
        fileIndex=fileIndex;
        decAddrWord=decAddrWord;
        endAddrWord=endAddrWord;
        decOpt=decOpt;
        imageWidth=imageWidth;
        imageHeight=imageHeight;
        destWidth=destWidth;
        destHeight=destHeight;
        return SUCCESS;
}
#else
UINT8 pbDecodeImage(
	UINT16 fileIndex,
	UINT32 *decAddrWord,
	UINT32 endAddrWord,
	UINT8 decOpt,
	UINT16 *imageWidth,
	UINT16 *imageHeight,
	UINT16 *destWidth,
	UINT16 *destHeight
) {
	UINT8 ret = FAIL;
	PBFile_t *pFile = &MediaInfo;
	UINT32 fileAddr;

	ExifImage_t mainPara, qvPara, thumbPara;
	ExifImage_t* pImagePara;
	endAddrWord=endAddrWord;
	if( pbFileInfoGet( fileIndex, pFile ) != SUCCESS ) {
		DBG_pbFuncTrack();
		return FAIL;
	}

	fileAddr = pbEditFileLoad( pFile, &mainPara, &thumbPara, &qvPara );
	if(!fileAddr){
		return FAIL;
	}
	if( ( decOpt & PB_DECODE_OPT_QVGA ) && exifAppN/*1: has Qv*/  ) {
		jpegQTableSet(85, NULL, NULL, 1);
		pImagePara = &qvPara;
	}
	else if( decOpt & PB_DECODE_OPT_THUMB ) {
		pImagePara = &thumbPara;
	}
	else {
		pImagePara = &mainPara;
	}
	*imageWidth = pImagePara->width;
	*imageHeight = pImagePara->height;
	ret = pbDecodeImageData( pImagePara, decAddrWord, fileAddr, destWidth, destHeight );
	return ret;
}
#endif
/**
 * @fn		  UINT8 pbDecodeImageData(Exif_Decompress_Parameter* imagePara, UINT32 *decAddrWord, UINT32 endAddrWord, UINT16 *destWidth, UINT16 *destHeight)
 * @brief	  decode image and do move it to center
 * @param	  [in] imagePara
 * @param	  [in] decAddrWord
 * @param	  [in] endAddrWord
 * @param	  [in] destWidth
 * @param	  [in] destHeight
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-12
 * @todo	  N/A
 * @bug 	  N/A
*/
#if (!STILL_PHOTOFRAME_DMY) //app not used
UINT8 pbDecodeImageData(
	ExifImage_t* imagePara,
	UINT32 *decAddrWord,
	UINT32 endAddrWord,
	UINT16 *destWidth,
	UINT16 *destHeight
) {
	UINT8 ret = SUCCESS;
	UINT8 opt;
	UINT16 srcWidth, srcHeight;
	UINT16 decWidth = *destWidth, decHeight = *destHeight;
	UINT16 MCURstNo = G_JpgRstMcuNum;
	//	UINT32 dramEndAddr = endAddr;

	dmyX = 0; dmyY = 0;
	if (imagePara->dataType == K_File_DataType_YUV420) {
		opt = 0x05;
//		exifFileFormat = 0x01;	 //set it here for save file after photoFrame combined
	} else if (imagePara->dataType == K_File_DataType_YUV422) {
		opt = 0x04;
//		exifFileFormat = 0x00;
	} else {
		return FAIL;
	}

	if( ( imagePara->width & 0x000f ) != 0x00 ) {
		OrderToAddr( srcWidth, 16 );
	} else {
		srcWidth = imagePara->width;
	}
	srcHeight = imagePara->height;
	imgScaleFac = pbCalImageScalFac4PhotoFrame( endAddrWord - *decAddrWord, srcWidth, srcHeight, &decWidth, &decHeight );
	//printf( "imgScaleFac: %bu\n", imgScaleFac );
	if(imgScaleFac == -1)
	{
		return FAIL;
	}
	//calculate the dmy first for judge if need move decAddr forward for moving image buffer to center.
	if( imgScaleFac ) {
		dmyX = ( ( srcWidth * ( UINT16 )imgScaleFac ) >> 3 ) & 7;
		dmyY = ( ( srcHeight * ( UINT16 )imgScaleFac ) >> 3 ) & 7;
		if( dmyX )
			dmyX = 8 - dmyX;
		if( dmyY )
			dmyY = 8 - dmyY;
	}
	//end
	//move dec buffer forward.
	if( (dmyX != 0 || dmyY != 0) && scaleChunk ) {
		gprmBlackFill( *decAddrWord, srcWidth, 2 * scaleChunk );
		*decAddrWord += 2 * ( UINT32 )scaleChunk * ( ( ( ( srcWidth * ( UINT16 )imgScaleFac ) >> 3 ) + 7 ) & ~7 );
	}
	//printf( "exifVlcAtOdd: %bu, opt:%bu, vlcAddr:x%lx\n", exifVlcAtOdd, opt, imagePara->addr);

	//fill the decode buffer first
	gprmBlackFill( *decAddrWord, ( decWidth + 7 ) & ~7, ( decHeight + 7 ) & ~7 );

	if( exifVlcAtOdd == 0 ) {
		ret = HAL_JpegDecode(imagePara->addr, srcWidth, srcHeight, *decAddrWord, imgScaleFac, opt, MCURstNo);
	}
	else {
		ret = HAL_JpegDecode(imagePara->addr, srcWidth, srcHeight, *decAddrWord, imgScaleFac, ( opt | 0x10 ), MCURstNo);
	}
	exifVlcAtOdd = 0;
	if( imgScaleFac ) {
		srcWidth = ( ( ( srcWidth * ( UINT16 )imgScaleFac ) >> 3 ) + 7 ) & ~7;
		srcHeight = ( ( ( srcHeight * ( UINT16 )imgScaleFac ) >> 3 ) + 7 ) & ~7;
	}
	*destWidth = srcWidth;
	*destHeight = srcHeight;

	//printf("dmyX: %bu, dmyY: %bu, (%u,%u)\n", dmyX, dmyY, srcWidth, srcHeight);
	dmyX = (dmyX + 3) & ~3; // gprm cpy should 2n alignment, 4n for offset
	dmyY = (dmyY + 3) & ~3;

	if(g_pbDecAlignOpt == 1){ // top_left
		g_pb_rec_RotOut.hOff=0;
		g_pb_rec_RotOut.vOff=0;
		g_pb_rec_RotOut.hSize=srcWidth - dmyX; // buffer shoud be 8 align
		g_pb_rec_RotOut.vSize=srcHeight - dmyY;
	}
	else{ // move to center
		g_pb_rec_RotOut.hOff=dmyX>>1;
		g_pb_rec_RotOut.vOff=dmyY>>1;
		g_pb_rec_RotOut.hSize=srcWidth - dmyX;
		g_pb_rec_RotOut.vSize=srcHeight - dmyY;
	}

	if( dmyX == 0 && dmyY == 0 || g_pbDecAlignOpt == 1 ) { // or dec to top_left
		return SUCCESS;
	}else {
		*decAddrWord = pbMoveImage2Center( *decAddrWord, endAddrWord, srcWidth, srcHeight, dmyX, dmyY );
	}

	return SUCCESS;
}

/**
 * @fn		  UINT8 pbCalImageScalFac4PhotoFrame(UINT32 bufSize, UINT16 srcWidth, UINT16 srcHeight, UINT16 *destWidth, UINT16 *destHeight)
 * @brief	  calculate scale factor and destination width/height
 * @param	  [in] bufSize
 * @param	  [in] srcWidth
 * @param	  [in] srcHeight
 * @param	  [in] destWidth
 * @param	  [in] destHeight
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-12
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 pbCalImageScalFac4PhotoFrame(
	UINT32 bufSize,
	UINT16 srcWidth,
	UINT16 srcHeight,
	UINT16 *destWidth,
	UINT16 *destHeight
) {
	UINT8 imageScaleFac = 0x08;
	UINT16 orgWidth = srcWidth, orgHeight = srcHeight;
	UINT32 orgbufSize = speBuffSizeGet();

	if(( UINT32 )srcWidth * srcHeight < 0x400000L)
	{
		orgbufSize -= 0x40000;
	}
	if(bufSize >= orgbufSize) //xian ++ 20090806 #38570
	{
		printf("bufsize by host\n");
		bufSize = orgbufSize;

	}
	printf("bufsize=0x%lx\n",bufSize);

	if( ( UINT32 )srcWidth * srcHeight > bufSize )
	{
		while( ( UINT32 )( ( srcWidth >> 3 ) * --imageScaleFac ) *	( ( srcHeight >> 3 ) * imageScaleFac ) > bufSize );
		if( imageScaleFac == 0 )
		{
			ASSERT(0, 1);
			imageScaleFac=-1;
			printf("buf isn't enough\n");
		}
	}
	else
	{

		imageScaleFac = 0;
	}
	//printf("imagescalefac=%bu\n",imageScaleFac);
	//added by zengping 08/10/06
	if( *destWidth != 0 )
	{
		 if(imageScaleFac == 0 || ( imageScaleFac != 0 && ( ( UINT16 )( srcWidth * ( UINT16 )imageScaleFac >> 3 ) >  *destWidth ) ) )
		 {
			imageScaleFac = ( *destWidth << 3 ) / srcWidth;
			if( srcWidth == ( *destWidth << 8 ) / imageScaleFac )
			{
				return imageScaleFac;
			}

		}
	}
	//end

	if( imageScaleFac ) {
		*destWidth = srcWidth * ( UINT16 )imageScaleFac >> 3;
		*destHeight = srcHeight * ( UINT16 )imageScaleFac >> 3;
	} else {
		*destWidth = srcWidth;
		*destHeight = srcHeight;
	}
	//printf("width=%d,height=%d\n",*destWidth,*destHeight);
	return imageScaleFac;
}

/**
 * @fn		  UINT32 pbMoveImage2Center(UINT32 startAddrWord, UINT32 freeEndAddrWord, UINT16 width, UINT16 height, UINT8 dmyX, UINT8 dmyY)
 * @brief	  move image to center
 * @param	  [in] startAddrWord
 * @param	  [in] freeEndAddrWord
 * @param	  [in] width
 * @param	  [in] height
 * @param	  [in] dmyX
 * @param	  [in] dmyY
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-12
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT32 pbMoveImage2Center(
	UINT32 startAddrWord,
	UINT32 freeEndAddrWord,
	UINT16 width,
	UINT16 height,
	UINT8 dmyX,
	UINT8 dmyY
) {
	UINT32 copyBufAddr;
	UINT16 tmpOftH = 0, tmpBlkH;
	UINT8 dtnvoft;

	copyBufAddr = startAddrWord + ( UINT32 )width * ( UINT32 )height;
	if( copyBufAddr + ( UINT32 )width * ( UINT32 )height < freeEndAddrWord ) {
		//move the whole buf directly

		//fill the copy buffer first
		gprmBlackFill( copyBufAddr, width, height );

		HAL_GprmCopy( startAddrWord, width, height, 0, 0,
						copyBufAddr, width, height, ( dmyX >> 1 ) & ~0x01, ( dmyY >> 1 ) & ~0x01,
						width - dmyX, height - dmyY, 0, 1 );
	} else {
		//buf is not enough, only move partial one time.
		copyBufAddr = startAddrWord - 2 * ( UINT32 )scaleChunk * width;  //move forward 2 * chunk, becasue temp buf for photoFrame is 2 * chunk
		copyBufAddr = ( copyBufAddr + 0x0f ) & ~0x0f;
		dtnvoft = ( dmyY >> 1 ) & ~0x01;
		while( tmpOftH < height ) {
			tmpBlkH = height - tmpOftH < scaleChunk ? ( height - tmpOftH ) : scaleChunk;
			gprmBlackFill( copyBufAddr + ( ( UINT32 )dtnvoft + tmpOftH ) * width, width, tmpBlkH );

			HAL_GprmCopy( startAddrWord + ( UINT32 )tmpOftH * width, width, tmpBlkH, 0, 0,
							copyBufAddr + ( ( UINT32 )dtnvoft + tmpOftH ) * width, width, tmpBlkH,
							( dmyX >> 1 ) & ~0x01, 0, width - dmyX, tmpBlkH,
							0, 1 );
			tmpOftH += tmpBlkH;
		}
	}
	return copyBufAddr;
}

/**
 * @fn		  void pbDisplayImage(UINT32 imageBufAddr, UINT16 imageWidth, UINT16 imageHeight)
 * @brief	  display playback image
 * @param	  [in] imageBufAddrWord
 * @param	  [in] imageWidth
 * @param	  [in] imageHeight
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void pbDisplayImage(
	UINT32 imageBufAddrWord,
	UINT16 imageWidth,
	UINT16 imageHeight
) {
	const UINT16 dispWidth = PB_DECODE_WIDTH;
	const UINT16 dispHeight = PB_DECODE_HEIGHT;
	const UINT32 startAddr = K_SDRAM_ImagePlaybackBufAddr;
	const UINT32 endAddr = K_SDRAM_TotalSize;
	UINT32 tempBufAddr = imageBufAddrWord + ( UINT32 )imageWidth * imageHeight;

	OrderToAddr( tempBufAddr, 16 );
	if( tempBufAddr > endAddr ) {
		tempBufAddr = imageBufAddrWord - 32 * 2 * imageWidth;
		tempBufAddr = tempBufAddr & ~15;
		if( tempBufAddr < startAddr ) {
			//printf( "out of memory!\n" );
			return;
		}
	}

	pbDispBuffSwitch();

	HAL_GprmScale( imageBufAddrWord, imageWidth, imageHeight,
					pbDispBuffGet( 1 ), dispWidth, dispHeight,
					tempBufAddr, 1 );
	dispShow(pbDispBuffGet(1), dispWidth, dispHeight, 100);
}

/**
 * @fn		  void pbPhotoFrameCombine(UINT32 imageBufAddrWord, UINT16 imageWidth, UINT16 imageHeight, UINT32 frameBufAddrWord, UINT16 frameWidth, UINT16 frameHeight)
 * @brief	  combine photoframe and jpeg file
 * @param	  [in] imageBufAddrWord
 * @param	  [in] imageWidth
 * @param	  [in] imageHeight
 * @param	  [in] frameBufAddrWord
 * @param	  [in] frameWidth
 * @param	  [in] frameHeight
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void pbPhotoFrameCombine(
	UINT32 imageBufAddrWord,
	UINT16 imageWidth,
	UINT16 imageHeight,
	UINT32 frameBufAddrWord,
	UINT16 frameWidth,
	UINT16 frameHeight
) {
	UINT32 tempAddr, yuvAddr;
	UINT16 overlap, idx;
	UINT16 width, height;
	UINT16 dtnvoft, blkHeight, tmpvoft;

	if( imageWidth < frameWidth ) {
		//yuvAddr = imageBufAddr + ( UINT32 )frameWidth * frameHeight;
		yuvAddr = frameBufAddrWord;
		OrderToAddr( yuvAddr , 16 );
		HAL_GprmScale(frameBufAddrWord, frameWidth, frameHeight,
						yuvAddr, imageWidth, imageHeight,
						frameBufAddrWord, 1);
		frameWidth = imageWidth;
		frameHeight = imageHeight;
		frameBufAddrWord = yuvAddr;
	}
	if( imageWidth == frameWidth && imageHeight == frameHeight ) {
		HAL_GprmCopy( frameBufAddrWord, frameWidth, frameHeight, 0, 0,
						imageBufAddrWord, imageWidth, imageHeight, 0, 0,
						imageWidth, imageHeight, ( UINT16 )0x80 + 0x10, 1 );
	} else {
		width = imageWidth - dmyX;
		height = imageHeight - dmyY;
		gprmPartialVScaleInit( frameWidth, frameHeight, width,
							height , scaleChunk );
		yuvAddr = imageBufAddrWord + ( UINT32 )( imageWidth + 32 ) * imageHeight;
		yuvAddr = ( yuvAddr + 15 ) & ~15;
		tempAddr = yuvAddr + 2 * ( UINT32 )scaleChunk * width;
		tempAddr = ( tempAddr + 15 ) & ~15;

		dtnvoft = ( dmyY + 1 ) >> 1;
		tmpvoft = 0;
		idx = 0;
		while( tmpvoft < height ) {
			blkHeight = ( tmpvoft + scaleChunk ) < height ? scaleChunk : ( height - tmpvoft );
			gprmPartialVScale( frameBufAddrWord, yuvAddr, tempAddr, idx++, &overlap );
			HAL_GprmCopy( yuvAddr + ( UINT32 )overlap * width, width, blkHeight, 0, 0,
							imageBufAddrWord, imageWidth, height,
							( dmyX + 1 ) >> 1, dtnvoft + tmpvoft, width, blkHeight,
							( UINT16 )0x80 + 0x10, 1 );
			tmpvoft += blkHeight;
		}
	}
}
#endif
/**
 * @fn		  void pbEncodeImage(UINT32 mainImageAddr, UINT16 width, UINT16 height, UINT16 destWidth, UINT16 destHeight)
 * @brief	  encode jpeg image which including photoframe
 * @param	  [in] mainImageAddrWord
 * @param	  [in] width
 * @param	  [in] height
 * @param	  [in] destWidth
 * @param	  [in] destHeight
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-12
 * @todo	  N/A
 * @bug 	  N/A
*/
#if STILL_PHOTOFRAME_DMY //spe used
void pbEncodeImage(
	UINT32 mainImageAddrWord,
	UINT16 width,
	UINT16 height,
	UINT16 destWidth,
	UINT16 destHeight
) {
        mainImageAddrWord=mainImageAddrWord;
        width=width;
        height=height;
        destWidth=destWidth;
        destHeight=destHeight;
}
#else
void pbEncodeImage(
	UINT32 mainImageAddrWord,
	UINT16 width,
	UINT16 height,
	UINT16 destWidth,
	UINT16 destHeight
) {
	UINT32 dramEndAddr = K_SDRAM_TotalSize - 256UL;//512 Bytes reserved for DMA
	UINT32 startAddr = K_SDRAM_ImagePlaybackBufAddr;
	UINT32 imageAddr, vlcAddr, qvAddr, tempAddr, hdrAddr, hdrSize;
	UINT32 qvWidth = PB_DECODE_WIDTH, qvHeight = PB_DECODE_HEIGHT;
	UINT32 vlcSize, size;
	UINT32 imgHeaderSize = 64UL * 1024UL;
	const UINT16 thumbWidth = 160, thumbHeight = 120;
	UINT32 qvTempAddr;
	exifJpgFileInfo_t jpgExifInfo;

	jpgExifInfo.fileFormat = 0x00;
	//encode MainImage
	imageAddr = pbEditImgMoveBufEnd( mainImageAddrWord, width, height, dramEndAddr );
	vlcAddr = startAddr + imgHeaderSize;

	jpgExifInfo.mainQidx = 90;
	jpegQTableSet(90, NULL, NULL, 1);
	HAL_JpgZFactorSet( 100, 100, width, height, destWidth, destHeight, 0x00 );

	HAL_JpegEncode( imageAddr, vlcAddr, 0, ( 0x20 | jpgExifInfo.fileFormat), 1 );
	HAL_JpegReadEncodeSize( &vlcSize );
	jpgExifInfo.mainWidth = destWidth;
	jpgExifInfo.mainHeight = destHeight;

	if(destWidth*3L>destHeight*4L)///16:9
	{
		//printf("w=%u,h=%u\n",width,height);
		qvTempAddr=imageAddr-PB_DECODE_WIDTH*PB_DECODE_HEIGHT;
		gprmBlackFill(qvTempAddr, PB_DECODE_WIDTH, PB_DECODE_HEIGHT);
		HAL_GprmScale( imageAddr, width, height,
		qvTempAddr+PB_DECODE_WIDTH*30L, PB_DECODE_WIDTH, 180,
		imageAddr, 1 );
		HAL_GprmCopy(qvTempAddr,PB_DECODE_WIDTH,PB_DECODE_HEIGHT,0,0,
						imageAddr,PB_DECODE_WIDTH,PB_DECODE_HEIGHT,0,0,
						PB_DECODE_WIDTH,PB_DECODE_HEIGHT,0,1
						);

	}
	else
	{
		HAL_GprmScale( imageAddr, width, height,
		imageAddr, PB_DECODE_WIDTH, PB_DECODE_HEIGHT,
		imageAddr, 1 );

	}
	//encode QVGA
	jpgExifInfo.qvQidx = 85;
	jpegQTableSet(85, NULL, NULL, 1);
	HAL_JpgZFactorSet( 100, 100, qvWidth, qvHeight, PB_DECODE_WIDTH, PB_DECODE_HEIGHT, 0x00 );

	qvAddr = vlcAddr + ( ( vlcSize + 1 ) >> 1 );
	HAL_JpegEncode( imageAddr, qvAddr, 0, ( 0x20 | jpgExifInfo.fileFormat), 1 );
	HAL_JpegReadEncodeSize( &size );
	jpgExifInfo.qvVlcAddrW = qvAddr;
	jpgExifInfo.qvSize = size;
	jpgExifInfo.qvWidth = PB_DECODE_WIDTH;
	jpgExifInfo.qvHeight = PB_DECODE_HEIGHT;

	//encode Thumb
	//scale down from qv to thumb
	tempAddr = dramEndAddr - ( UINT32 )PB_DECODE_WIDTH * PB_DECODE_HEIGHT;
	tempAddr = tempAddr & ~7;
	HAL_GprmScale( imageAddr, qvWidth, qvHeight,
	imageAddr, thumbWidth, thumbHeight,
	tempAddr, 1 );

	jpgExifInfo.thbQidx = 85;
	jpegQTableSet(85, NULL, NULL, 1);
	HAL_JpgZFactorSet( 100, 100, thumbWidth, thumbHeight, thumbWidth, thumbHeight, 0x00 );

	qvAddr = qvAddr + ( ( size + 1 ) >> 1 );
	HAL_JpegEncode( imageAddr, qvAddr, 0, ( 0x20 | jpgExifInfo.fileFormat), 1 );
	HAL_JpegReadEncodeSize( &size );
	jpgExifInfo.thbVlcAddr = qvAddr;
	jpgExifInfo.thbSize = size;
	jpgExifInfo.thbWidth = thumbWidth;
	jpgExifInfo.thbHeight = thumbHeight;
	jpgExifInfo.thbSize = size;


	//Make File
	hdrAddr = qvAddr + ( UINT32 )((( size + 1 ) >> 1 ) + 15 ) & ~15;
	if( hdrAddr + imgHeaderSize < dramEndAddr ) {
		//EXIFHDR_Initialize_AllExifHeader(hdrAddr);
		//hdrSize = EXIFHDR_LoadExifHeader( vlcAddr, hdrAddr, tmbVlcSize );
		//new exif lib
		jpgExifInfo.mainVlcAddrW = vlcAddr;
		jpgExifInfo.rotOpt = pbRotModeGet(2);
		jpgExifInfo.dbgInfoAddr = 0;
		hdrSize = exifCreateFile(&jpgExifInfo);
		pbEditFileMake(vlcAddr - (hdrSize >> 1), vlcSize + hdrSize);
	} else {
		//printf( "Out of Memory!\n" );
	}
}
#endif

