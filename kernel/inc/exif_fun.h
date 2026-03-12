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
 * @file		exif_fun.h
 * @brief		exif standard 2.2
 * @author		hy.ren
 * @since		2008-03-14
 * @date		2008-10-31
 */
/**************************************************************************
* 						   H E A D E R	 F I L E S
**************************************************************************/

 
#ifndef _EXIF_FUN_H_
#define _EXIF_FUN_H_
#include "common.h"

typedef enum {
	EXIF_BYTE_ORDER_MOTOROLA = 0x4d4d,
	EXIF_BYTE_ORDER_INTEL = 0x4949,
} ExifByteOrder;

//void exif_File_RW(const char *filePath,long offset,int orign,char bRead,char *buf,int size);
/**
* @fn UINT8 sp1kPbDistortSave(PBFile_t* pFile, UINT8 speDisIdx)
* @brief   Set specified effect of current image and create a new file
* @param[in] fileIndex	 specify the file order in playback mode
* @param[in] speDisIdx	 specified the image effect to set
* @retval error_code	success: 0, fail: 1
*/			
short exif_get_sshort (const unsigned char *buf, ExifByteOrder order);
/**
* @fn UINT8 sp1kPbDistortSave(PBFile_t* pFile, UINT8 speDisIdx)
* @brief   Set specified effect of current image and create a new file
* @param[in] fileIndex	 specify the file order in playback mode
* @param[in] speDisIdx	 specified the image effect to set
* @retval error_code	success: 0, fail: 1
*/			
unsigned short exif_get_short (const unsigned char *buf, ExifByteOrder order);
/**
* @fn UINT8 sp1kPbDistortSave(PBFile_t* pFile, UINT8 speDisIdx)
* @brief   Set specified effect of current image and create a new file
* @param[in] fileIndex	 specify the file order in playback mode
* @param[in] speDisIdx	 specified the image effect to set
* @retval error_code	success: 0, fail: 1
*/
unsigned long exif_get_slong (const unsigned char *b, ExifByteOrder order);
/**
* @fn UINT8 sp1kPbDistortSave(PBFile_t* pFile, UINT8 speDisIdx)
* @brief   Set specified effect of current image and create a new file
* @param[in] fileIndex	 specify the file order in playback mode
* @param[in] speDisIdx	 specified the image effect to set
* @retval error_code	success: 0, fail: 1
*/
long exif_get_long (const unsigned char *buf, ExifByteOrder order);

unsigned short exif_swap_ushort (unsigned short value);
unsigned long exif_swap_ulong (unsigned long value);

UINT16 swapWord(UINT16 x);
UINT32 swapDWord(UINT32 x);
void short2char(unsigned char *buf,unsigned short value);
void long2char(unsigned char *buf,unsigned long value);

UINT32 exif_longValue_get(UINT32 x, ExifByteOrder byteorder);
UINT16 exif_shortValue_get(UINT16 x, ExifByteOrder byteorder);

#endif


