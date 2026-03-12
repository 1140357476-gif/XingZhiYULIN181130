/**************************************************************************
 *
 *        Copyright (c) 2007-2008 by Sunplus mMedia Inc., Ltd.
 *
 *  This software is copyrighted by and is the property of Sunplus
 *  mMedia Inc., Ltd. All rights are reserved by Sunplus mMedia
 *  Inc., Ltd. This software may only be used in accordance with the
 *  corresponding license agreement. Any unauthorized use, duplication,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of Sunplus mMedia Inc., Ltd.
 *
 *  Sunplus mMedia Inc., Ltd. reserves the right to modify this
 *  software without notice.
 *
 *  Sunplus mMedia Inc., Ltd.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 **************************************************************************/
/**
 * @file		sp1k_exif_api.h
 * @brief		sp1k exif module header file.
 * @author		Xian Xin
 * @since		2008-10-27
 * @date		2008-10-27
 */
#ifndef __SP1K_EXIF_API_H__
#define __SP1K_EXIF_API_H__

#include "exif_entry.h"
#include "exif_param.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/

/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/
#ifdef SP1K_DOXY_GEN

/**
 * @fn UINT8 sp1kExifParseStart (exifCallBackFun func,ExifImage_t *image,UINT32 addrByte,UINT32 size);
 * @brief	exif parser start 
 * @param[in] func:   call back function 
 * @param[in] image: JPG image data struct 
 * @param[in] addrByte:  JPG file data srcouce
 * @param[in] size     decode size   
 * @retval UINT8   success: 0, fail: 1
 * @see also 		none
 */
UINT8 sp1kExifParseStart (exifCallBackFun func,ExifImage_t *image,UINT32 addrByte,UINT32 size);

/**
 * @fn UINT8 sp1kExifSetIFDData (ExifIfd exififdtype, ExifTag  exiftag,UINT32 va_count,...);
 * @brief	set EXIF tag's value
 * @param[in]  exififdtype   specified as IFD0, EXIF IFD, IFD1,GPS or Interoperability IFD  
 * @param[in] exiftag  specify IFD's tag id 
 * @param[in] va_count
 * @param[in] variable parmeter 
 * @retval 	   TRUE/FALSE
 * @see also 	  none 
 */
UINT8 sp1kExifSetIFDData (ExifIfd exififdtype, ExifTag  exiftag,UINT32 va_count,...);

/**
 * @fn void sp1kExifGetMainInfo(ExifImage_t *mainImage);
 * @brief	Set exif user comment code. 
 * @param[in] cmCode   specify exif user comment code 
 * @param[in] en   enable or disable exif user comment tag 
 * @retval 		TRUE/FALSE
 * @see also 		none 
 */
void sp1kExifGetMainInfo(ExifImage_t *mainImage);

/**
 * @fn void sp1kExifGetThumbInfo (ExifImage_t *thumbImage);
 * @brief	get thumbnail infomation
 * @param[in] mainImage		main Image data struct 
 * @retval 	none 
 * @see also 
 */
void sp1kExifGetThumbInfo (ExifImage_t *thumbImage);
/**
 * @fn void sp1kExifGetQVInfo(ExifImage_t *qvImage);
 * @brief	get qv infomation 
 * @param[in] qvImage	qv image data struct 
 * @retval				none
 * @see also 				none 
 */
void sp1kExifGetQVInfo(ExifImage_t *qvImage);
/**
 * @fn UINT32 sp1kExifCreateFile( UINT32 vlcStart,UINT32 exifAddr, UINT32 thubAddr, UINT32 thubSize );
 * @brief		create JPG file 
 * @param[in] 	vlcStart	vlc data start address 
 * @retval 		SUCCESS/FAIL
 * @see also		none 
 */
UINT32 sp1kExifCreateFile(const exifJpgFileInfo_t *jpgInfo);


#else
#define sp1kExifParseStart				exifParserStart
#define sp1kExifSetIFDData				exifSetIFDData
#define sp1kExifGetMainInfo				exifGetMainInfo
#define sp1kExifGetThumbInfo			exifGetThumbInfo
#define sp1kExifGetQVInfo				exifGetQVInfo
#define sp1kExifCreateFile				exifCreateFile

#endif

#endif  /* __SP1K_EXIF_API_H__ */
