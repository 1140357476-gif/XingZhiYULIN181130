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
 * @file      tq.zhang
 * @brief     date stamp for video record
 * @author    tq.zhang
 * @since     2010-05-13
 * @date      2010-05-13
*/

#ifndef __VIDEO_DTSTMP_H__
#define __VIDEO_DTSTMP_H__
#include "stamp.h"
/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define STAMP_RESERVED_FRAME_SIZE ((UINT32)20*100*1024)
#define VD_STAMP_MODE_MASK		0x0f
#define VD_STAMP_RECMODE_MASK	0xf0

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct
{
	UINT8 *text;
	UINT16 width,height;
	UINT16 xoff,yoff;

}video_osd_text_t;
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void videoRecStampCfg(UINT8 mode, UINT8 aligFmt, UINT8 recMode);
UINT8 videoRecStampImgResLoad(UINT16 resFileId, UINT16 stampW, UINT16 stampH);
void videoRecStampStsSet (UINT8 en);
void videoCapStampStsSet (UINT8 en);
UINT8 videoRecStampStsGet(void);
void videoRecStampDateFmtSet(UINT16 fmt);
UINT16 videoRecStampDateFmtGet(void);
void videoRecStampInit();
void videoRecStampLoad(UINT8 idx);
UINT8 videoRecStampModeGet(void);
UINT8 videoRecStampRecModeGet(void);
UINT8 videoRecStampAligFmtGet(void);
void videoRecDateStampLoadInit();
//void videoTimeNext(dateStc_t dateInfNext);

UINT8 videoRecStampSet(UINT8 pasteThr, UINT16 stampW, UINT16 stampH, UINT16 fontW,UINT16 fontH);
video_osd_text_t* videoRecOsdTextGet(stamp_text_type type);
void videoCaptureStampSetIntr();

#endif  /* __TEMPLATE_HDR_H__ */

