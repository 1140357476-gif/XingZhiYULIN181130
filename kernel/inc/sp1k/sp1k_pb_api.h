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
#ifndef __SP1K_PB_API_H__
#define __SP1K_PB_API_H__

/** \file sp1k_pb_api.h
 * @brief		Spca1528 playback related interface
 */

#include "pb_api.h"
#include "fileindex.h"
#include "sp1k_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define SP1K_PB_FILE_TYPE_JPG			((UINT8)0x01)
#define SP1K_PB_FILE_TYPE_AVI			((UINT8)0x02)
#define SP1K_PB_FILE_TYPE_WAV			((UINT8)0x03)
#define SP1K_PB_FILE_TYPE_WGI			((UINT8)0x04)
#define SP1K_PB_FILE_TYPE_UNKNOWN	((UINT8)0xFF)
#define SP1K_PB_FILE_TYPE_THUMB 		((UINT8)0x00)
	

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#ifdef SP1K_DOXY_GEN

/**
 * @fn void sp1kPbInit(void)
 * @brief	Initialize playback mode                          
 * @retval none
 */
void sp1kPbInit(void);

/**
 * @fn UINT8 sp1kPbParamSet(UINT8 paraId, UINT16 value)
 * @brief	Specify some parameter to playback function
 * @param[in] paraId    specify the parameter ID
 * @param[in] value    specify the paramter value
 * @retval error_code    success: 0, fail: 1
 * @see enum "PBParam_e"
 */
UINT8 sp1kPbParamSet(UINT8 paraId, UINT16 value);

/**
 * @fn void sp1kPbDispInit(UINT32 srcAddr, UINT16 srcHSize, UINT16 srcVSize)
 * @brief	protect(freeze) the display image buffer before switch to playback mode
 * @param[in] srcAddr    specify the address of current display buffer
 * @param[in] srcHSize   specify the width of current display image
 * @param[in] srcVSize   specify the height of current display image
 * @retval none
 */
void sp1kPbDispInit(UINT32 srcAddr, UINT16 srcHSize, UINT16 srcVSize);

/**
 * @fn UINT8 sp1kPbFileInfoGet(UINT16 FileIndex, PBFile_t* FileInfo)
 * @brief	Get the information of the specified file
 * @param[in] FileIndex    specify the file index
 * @param[out] FileInfo    point to the struct which store the specified file information
 * @retval error_code    success: 0, fail: 1
 */
UINT8 sp1kPbFileInfoGet(UINT16 FileIndex, PBFile_t* FileInfo);

/**
 * @fn UINT16 sp1kPbFileCntGet(void)
 * @brief	Get the total file number of current directory
 * @retval FileCount the the total file count
 */
UINT16 sp1kPbFileCntGet(void);

/**
 * @fn void sp1kPbDZoomFactorSet(UINT16 Factor)
 * @brief	Set the digital zoom fact of the current image in playback mode       
 * @param[in] Factor    Specify the zoom fact, The range is from 100(1.0x) to 400(4.0x)
 * @retval none
 */
void sp1kPbDZoomFactorSet(UINT16 Factor);	

/**
 * @fn UINT8 sp1kPbImgPanSet(SINT16 stepX, SINT16 stepY)
 * @brief	specify the move step in pan mode, it will auto adjust when the step is over range.       
 * @param[in] stepX    Specify the X step in pan mode
 * @param[in] stepY    Specify the Y step in pan mode
 * @retval err    0=OK,
 *                     bit0=X_UNDERFLOW, bit1=X_OVERFLOW,
 *                     bit2=Y_UNDERFLOW, bit3=Y_OVERFLOW.
 */
UINT8 sp1kPbImgPanSet(SINT16 stepX, SINT16 stepY);

/**
 * @fn UINT32 sp1kPbDispBuffGet(UINT8 opt)
 * @brief	Get the active / inactive playback display buffer address
 *\n        Active display buffer: the current display show buffer
 *\n        Inactive display buffer: another display display buffer not be used currently
 * @param[in] opt    specify active / inactive playback display buffer
 *\n                        1: active
 *\n                        0: inactive
 * @retval DispAddr    active / inactive playback display buffer address
 * @see sp1kPbDispBuffSwitch()
 */				
UINT32 sp1kPbDispBuffGet(UINT8 opt);

/**
 * @fn UINT32 sp1kPbDispBuffSwitch(void)
 * @brief	Switch the display buffer in playback mode
 * @retval DispAddr    the active display buffer address
 * @see sp1kPbDispBuffGet()
 */				                	 
UINT32 sp1kPbDispBuffSwitch(void);

/**
 * @fn UINT8 sp1kPbSingleImgPlay(UINT16 FileIndex)
 * @brief	play the specified image file in playback mode
 * @param[in] FileIndex    specify the image file index to play
 * @retval error_code    success: 0, fail: 1
 */				
UINT8 sp1kPbSingleImgPlay(UINT16 FileIndex);

/**
 * @fn UINT8 sp1kPbVideoFramePlay(UINT16 FileIndex)
 * @brief	play the specified video file in playback mode
 * @param[in] FileIndex    specify the video file index to play
 * @retval error_code    success: 0, fail: 1
 */				     
UINT8 sp1kPbVideoFramePlay(UINT16 FileIndex);

/**
 * @fn UINT8 sp1kPbVoiceMemoCheck(UINT16 FileIndex)
 * @brief	Check if there is a voice memo in specified image file
 * @param[in] FileIndex    specify the image file index to check
 * @retval status    true: 1, false: 0
 */				           
UINT8 sp1kPbVoiceMemoCheck(UINT16 FileIndex);

/**
 * @fn void sp1kPbDecodeSizeSet(UINT16 Width, UINT16 Height)
 * @brief	Specify the width and height of the image to decode
 * @param[in] Width    specify the width of the image
 * @param[in] Height    specify the height of the image
 * @retval none
 * @see sp1kPbDecodeSizeGet()
 */				                	
void sp1kPbDecodeSizeSet(UINT16 Width, UINT16 Height);

/**
 * @fn void sp1kPbDecodeSizeGet(UINT16* Width, UINT16* Height)
 * @brief	Get the width and height of the image last decoded
 * @param[out] Width    specify the width of the image last decoded
 * @param[out] Height    specify the height of the image last decoded
 * @retval none
 * @see sp1kPbDecodeSizeSet()
 */				                	   
void sp1kPbDecodeSizeGet(UINT16* Width, UINT16* Height);

/**
 * @fn void sp1kPbDispQVSizeGet(UINT16* Width, UINT16* Height)
 * @brief	Get the width and height of the QV 
 * @param[out] Width    specify the width of the QV
 * @param[out] Height    specify the height of the QV
 * @retval none
 * @see
 */	
void sp1kPbDispQVSizeGet(UINT16 *Width, UINT16 *Height);

/**
 * @fn UINT8 sp1kPbMultiPlayLayoutInit(UINT8 LayoutType, UINT16 FileIndex)
 * @brief	Calculate the layout offset and get the information of all nine images before multiplay show
 * @param[in] LayoutType    specify the type of multiplay layout
 *\n                                    PB_MULTIPLAY_LAYOUT_9: 9 images in the same page
 *\n                                    PB_MULTIPLAY_LAYOUT_4: 4 images in the same page
 * @param[in] FileIndex    specify the index of current image
 * @retval error_code    success: 0, fail: 1
 */				                	    
UINT8 sp1kPbMultiPlayLayoutInit(UINT8 LayoutType, UINT16 FileIndex);


/**
 * @fn void sp1kPbMultiPlay(void)
 * @brief	Show the multiplay images, this function must be called after "sp1kPbMultiPlayLayoutInit"
 * @retval none
 * @see sp1kPbMultiPlayLayoutInit()
 */				                	    
void sp1kPbMultiPlay(void); 

/**
 * @fn UINT8 sp1kPbMultiPlayShift(UINT16* fileIdx, UINT8 shift)
 * @brief	Shift the focus object to another image by directions
 * @param[out] fileIdx    specify the index of current file
 * @param[in] shift    specify the directions to shift
 *\n                          PB_SHIFT_UP: shift up
 *\n                          PB_SHIFT_DOWN: shift down
 *\n                          PB_SHIFT_LEFT: shift left
 *\n                          PB_SHIFT_RIGHT: shift right
 * @retval error_code    success: 0, fail: 1
 * @see sp1kPbMultiPlay()
 */				                	    
UINT8 sp1kPbMultiPlayShift(UINT16* fileIdx, UINT8 shift);

/**
 * @fn void sp1kPbMultiPlayObjFrameDraw(UINT8 idx, UINT8 enable)
 * @brief	Show the osd selected frame for specified image in multiplay mode
 * @param[in] idx    specify file index
 * @param[in] enable    configure the osd selected frame, 1: enable, 0: disable
 * @retval none
 * @see sp1kPbBufHsizeSet()
 */				                	    
void sp1kPbMultiPlayObjFrameDraw(UINT8 idx, UINT8 enable);

/**
 * @fn void sp1kPbBufAddrSet(UINT32 addr)
 * @brief	Set the playback display buffer address
 * @param[in] addr    specify display buffer address
 * @retval none
 * @see sp1kPbBufHsizeSet()
 */				                	    
void sp1kPbBufAddrSet(UINT32 addr);

/**
* @fn void sp1kPbBufHsizeSet(UINT16 width)
* @brief   Set the playback display horizontal size
* @param[in] width    specify display horizontal size
* @retval none
* @see sp1kPbBufAddrSet()
*/									  
void sp1kPbBufHsizeSet(UINT16 width);
  
/**
* @fn UINT8 sp1kPbEffectSet(PBFile_t* pFile, PBEffectList_t imgEffect)
* @brief   Set specified effect of current image and create a new file
* @param[in] pFile	 point to the file information of current image
* @param[in] imgEffect	 specified the image effect to set
* @retval error_code	success: 0, fail: 1
*/									 
UINT8 sp1kPbEffectSet(PBFile_t* pFile, PBEffectList_t imgEffect);

/**
* @fn UINT8 sp1kPbImgResize(PBFile_t* pFile, UINT16 dstWidth, UINT16 dstHeight)
* @brief   resize the current file to specified size, and create a new file
* @param[in] pFile   point to the file information of current image 
* @param[in] dstWidth   specify the width of destination image file
* @param[in] dstHeight   specify the height of destination image file
* @retval error_code	success: 0, fail: 1
*/									 
UINT8 sp1kPbImgResize(PBFile_t* pFile, UINT16 dstWidth, UINT16 dstHeight);

/**
* @fn UINT8 sp1kPbFilePlayFadeIn(UINT16 Time)
* @brief   play fade in effect when switch to a new file in playback mode
* @param[in] Time	 specify the total time of fade in process, the unit time is "ms"
* @retval error_code	success: 0, fail: 1
*/									 
UINT8 sp1kPbFilePlayFadeIn(UINT16 Time);

/**
* @fn UINT32 sp1kPbFreeBuffGet(void)
* @brief   Get the free buffer address in playback mode
* @param[in] none
* @retval FreeAddr    The free buffer address
*/									 
UINT32 sp1kPbFreeBuffGet(void);

/**
* @fn void sp1kPbFileOrdertableInit(void)
* @brief   Initialize the file order table, This table store the true file index in dram, the max number is 9999
*\n         eg:There is only one file named "SUNP0011.JPG" in SD card, so we say,
*\n         The file order of this file is "1", and the true file index of this file is "11".
* @retval none
* @see sp1kPbLastIndexClear(), sp1kPbFileOrderReset(), sp1kPbFileIndexByOrderGet()
*/									 
void sp1kPbFileOrdertableInit(void);

/**
* @fn void sp1kPbLastIndexClear(void)
* @brief   remove the last file index in file order table
* @retval none
* @see sp1kPbFileOrdertableInit(), sp1kPbFileOrderReset, sp1kPbFileIndexByOrderGet()
*/									 
void sp1kPbLastIndexClear(void);

/**
* @fn void sp1kPbFileOrderReset(UINT16 index)
* @brief   remove the specified file index in file order table , 
* \n         eg: After file delete, you may remove the file index information of that file in file order table
* @param[in] index     specify the file index have to remove from the file order table
* @retval none
* @see sp1kPbFileOrdertableInit(), sp1kPbLastIndexClear(), sp1kPbFileIndexByOrderGet()
*/									 
void sp1kPbFileOrderReset(UINT16 index);

/**
* @fn UINT16 sp1kPbFileIndexByOrderGet(UINT16 index) 
* @brief   get the specified file true index by file order in playback mode
* @param[in] Index     specify the file order in playback mode
* @retval FileIndex    the true index of the specified file
* @see sp1kPbFileOrdertableInit(), sp1kPbLastIndexClear(), sp1kPbFileOrderReset()
*/									 
UINT16 sp1kPbFileIndexByOrderGet(UINT16 index); 

/**
* @fn UINT16 sp1kPbFillBlack(UINT32 dstAddr, UINT16 imgW, UINT16 imgH)
* @brief   fill black to display image layer
* @param[in] dstAddr     specify the display buffer address
* @param[in] imgW     specify the width of display
* @param[in] imgH     specify the height of display
* @retval none
*/									 
void sp1kPbFillBlack(UINT32 dstAddr, UINT16 imgW, UINT16 imgH);

/**
* @fn UINT8 sp1kPbDistortSave(PBFile_t* pFile, UINT8 speDisIdx)
* @brief   Set specified effect of current image and create a new file
* @param[in] fileIndex	 specify the file order in playback mode
* @param[in] speDisIdx	 specified the image effect to set
* @retval error_code	success: 0, fail: 1
*/									 
UINT8 sp1kPbDistortSave(UINT16 fileIndex, UINT8 speDisIdx);

/**
* @fn UINT8 sp1kPbDistortView(PBFile_t* pFile, UINT8 speDisIdx)
* @brief   Quick view effect of current image
* @param[in] fileIndex	 specify the file order in playback mode
* @param[in] speDisIdx	 specified the image effect to set
* @retval error_code	success: 0, fail: 1
*/
UINT8 sp1kPbDistortView(UINT8 speDisIdx);

/**
* @fn UINT8 sp1kdecodePhotoFrameImage( UINT32 frameResIdx, UINT32 *decAddr, UINT32 endAddr, UINT16 *frameWidth, UINT16 *frameHeight );

* @brief   decode the photoFrame
* @param[in] frameResIdx	 specify the index of the photoframe
* @param[in] decAddr		point to the start adderesee of the decoded frame 
* @param[in] endAddr	   specified the end adderess of the decoded frame
* @param[out] frameWidth   point to  the width of the photoframe
* @param[out] frameHeight  point to the height of the photoframe
* @retval error_code	success: 0, fail: 1
*/
UINT8 sp1kdecodePhotoFrameImage( UINT32 frameResIdx, UINT32 *decAddr, UINT32 endAddr, UINT16 *frameWidth, UINT16 *frameHeight );

/**
* @fn void sp1kSpeParamSet(UINT8 idx,UINT16 value)
* @brief   Set parameters for distortion
* @param[in] idx	 
* @param[in] value	
* @retval void
*/
void sp1kSpeParamSet(UINT8 idx,UINT16 value);
/**
* @fn UINT16 sp1kSpeParamGet(UINT8 idx)
* @brief   Get parameters for distortion
* @param[in] idx	 
* @retval param value	
*/
UINT16 sp1kSpeParamGet(UINT8 idx);
/**
* @fn UINT8 sp1kpbDecRotModeGet(void)
* @brief   get the mode to know that if the image is rotate
* @retval PB_ROT_0:0, PB_ROT_90: 5,PB_ROT_180: 3,PB_ROT_270: 6
*/
UINT8 sp1kpbDecRotModeGet(void);
/**
* @fn UINT8 sp1kpbCurFoldFileSpaceGet(UINT32 *fileSize,UINT8 fileType)
* @brief   get current folder file total size
* @retval SUCCESS/FAILE
*/
UINT8 sp1kpbCurFoldFileSpaceGet(UINT32 *fileSize,UINT8 fileType);

/**
* @fn UINT8 sp1kpbDecRotModeGet(UINT8 option);
* @brief   select  Gprm or JPG rotation
* @param     UINT8 option
 							option =0; GPRM rotation
 							option =1; JPG rotation
* @retval    return = SUCCESS / FAIL
*/
UINT8 sp1kpbDecRotModeGet(UINT8 option);
/**
* @fn void sp1kPbCallbackReg(UINT32 id,void *pfunct);
* @brief   register pbabort function
* @param[in]  id
* @param[in]  *pfunct							
* @retval    return: none
*/
void sp1kPbCallbackReg(UINT32 id,void *pfunct);

/**
* @fn UINT8 sp1kPbEditSave( pbEditType_t imgEffect,PbCropImage_t cropRec );
* @brief   save image
* @param[in]  imgEffect
* @param[in]  cropRec
* @retval    return: none
*/
UINT8 sp1kPbEditSave(PBFile_t* pFile, pbEditType_t imgEffect,PbCropImage_t cropRec );

/**
* @fn UINT8 sp1kPbEdit_ImgDec(PBFile_t* pFile,pbEditType_t imgEffect,PbCropImage_t *cropRec);
* @brief   decode image
* @param[in]  pFile
* @param[in]  imgEffect
* @param[in]  cropRec
* @retval    return: none
*/
UINT8 sp1kPbEdit_ImgDec(PBFile_t* pFile,pbEditType_t imgEffect,PbCropImage_t *cropRec);

#else
#define sp1kPbInit                				pbInit
#define sp1kPbParamSet					pbParamSet
#if 0  //not used cx mask
#define sp1kPbParamGet					pbParamGet
#endif
#define sp1kPbDispInit					pbDispInit
#define sp1kPbFileInfoGet    				pbFileInfoGet
#define sp1kPbFileCntGet        				pbFileCntGet
#define sp1kPbDZoomFactorSet			pbDZoomFactorSet
#define sp1kPbImgPanSet					pbImgPanSet
#define sp1kPbDispBuffGet         			pbDispBuffGet
#define sp1kPbSingleImgPlay       			pbSingleImgPlay
#define sp1kPbVideoFramePlay      		pbVideoFramePlay
#define sp1kPbVoiceMemoCheck      		pbVoiceMemoCheck
#define sp1kPbDecodeSizeSet       			pbDecodeSizeSet
#define sp1kPbDispBuffSwitch      			pbDispBuffSwitch
#define sp1kPbDecodeSizeGet       			pbDecodeSizeGet
#define sp1kPbDispQVSizeGet					pbDispQVSizeGet
#define sp1kPbMultiPlayLayoutInit 		pbMultiPlayLayoutInit
#define sp1kPbMultiPlay           			pbMultiPlay
#define sp1kPbMultiPlayShift 				pbMultiPlayShift
#define sp1kPbMultiPlayObjFrameDraw		pbMultiPlayObjFrameDraw
#define sp1kPbBufAddrSet          			HAL_PbBufAddrSet
#define sp1kPbBufHsizeSet         			HAL_PbBufHsizeSet
#define sp1kPbEffectSet           			pbEffectSet
#define sp1kPbFilePlayFadeIn 			pbFilePlayFadeIn
#define sp1kPbFreeBuffGet 				pbFreeBuffGet
#define sp1kPbImgResize 					pbImgResize
#define sp1kPbFileOrdertableInit  			DOS_FileOrderTblInit
#define sp1kPbLastIndexClear				DOS_FileLastIndexClear
#define sp1kPbFileOrderReset				DOS_FIleOrderRst
#define sp1kPbFileIndexByOrderGet		DOS_FileIndexGet
#define sp1kPbUserStartImgGet                   pbUserStartImgGet
#define sp1kPbImgRotate                 		pbImgRotate
#define sp1kPbFillBlack					gprmBlackFill
#define sp1kdecodePhotoFrameImage 		pbDecodePhotoFrameImage
#define sp1kpbCurFoldFileSpaceGet		pbCurFoldFileSpaceGet
#define sp1kPbCallbackReg				pbCallbackReg
#define sp1kPbEdit_ImgDec			pbEdit_ImgDec
#define sp1kPbEditSave 				pbEdit_Save
#define sp1kpbDecRotModeGet			pbDecRotModeGet 
#define sp1kPbMultiPosGet					pbMultiPosGet

#if 0  //not used cx mask
#define sp1kpbDZoomKeySet				pbDZoomKeySet
#define sp1kpbRotModeGet 			pbRotModeGet
#endif 
#define sp1kpbDZoomKeyGet				pbDZoomKeyGet
#if 0//def MULTI_FOLDER_AS_ONE  //xian ++ 20080806
#define sp1kPbFileTotalCntGet           pbFileTotalCntGet
#define sp1kPbFirstFileIndexGet         pbFirstFileIndexGet
#endif

#define sp1kPbDistortView				speDistortView
#define sp1kPbDistortSave				speDistortSave
#define sp1kSpeParamSet					speParamSet 
#define sp1kSpeParamGet					speParamGet
#define sp1kSpeBuffSizeSet					speBuffSizeSet
#define sp1kSpeBuffSizeGet					speBuffSizeGet

#define sp1kPbEditUserCommentCodeModify		pbEditUserCommentCodeModify
#define sp1kPbEditUserCommentCodeGet		pbEditUserCommentCodeGet

#endif

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

#endif //__SP1K_PB_API_H__

