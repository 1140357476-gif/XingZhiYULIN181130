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
 * @file         video_slclip.h
 * @brief       videoseamless loop  rec
 * @author    rf.wang
 * @since      2011-03-14
 * @date       2011-03-14
*/

#ifndef __VIDEO_SLCLIP_H__
#define __VIDEO_SLCLIP_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
//#define VIDEO_SLCLIP_DEBUG_LEVEL 0
//#define VIDEO_SLCLIP_FILE_NOT_DELETE_READONLY 1
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 /* typedef struct seamLessLpRecfile
{
   UINT8 filename[255][12];
   UINT16 index;
   UINT16 indexDelete;
}seamLessLpRecfile_t;*/
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
//UINT8 videoSLRecCloseFile(void);
//UINT8 videoSLRecOpenFile(void);
//void videoSLRecInit(void);
//UINT8 videoSLRecInitBF (void);
void videoSeamlessLPRecStsSet(UINT8 en);
void videoSeamlessLPRecFilesizeSet(UINT32 filesize);
void videoSeamlessLPRecTimeSet(UINT32 rectime);
UINT8 videoSLFileRename();
UINT8 videoSLRecMemCheck();
#endif  /* __TEMPLATE_HDR_H__ */
