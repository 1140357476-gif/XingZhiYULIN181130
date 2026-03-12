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
 * @file      video_cap_vlcbuf.h
 * @brief     
 * @author    hy.ren
 * @since     2010-05-12
 * @date      2010-05-12
*/


#ifndef __VIDEO_CAP_VLCBUF_H__
#define __VIDEO_CAP_VLCBUF_H__

#include "video_capdef.h"

typedef struct QueParam
{
	UINT32 addr;
	UINT32 size;
	UINT32 rsvSize;
	UINT32 capacity;
}QueParam_t;


void speedyCaptureStillQueInit(vlcBufList_t *pVlcBufQue);
void intrSpeedyCaptureStillBufSet(vlcBufList_t *vlcQue);

void intrSpeedyCaptureClipBufSet(vlcBufList_t *tmpQue,vlcBufList_t *pCashQue,UINT8 IsHit);
void speedyCaptureClipQueInit(vlcBufList_t *pVlcCashQue, vlcBufList_t *pVlcBufQue);


#endif/*__VIDEO_CAP_VLCBUF_H__*/

