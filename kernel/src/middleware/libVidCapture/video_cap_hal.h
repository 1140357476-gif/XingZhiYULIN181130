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
 * @file	  video_cap_hal.h
 * @brief	  video capture funtion 
 * @author	  hy.ren
 * @since	  2010-05-13
 * @date	  2010-05-13
*/

#ifndef __VID_CAP_HAL_H__
#define __VID_CAP_HAL_H__

#include "general.h"
#include "common.h"
#include "reg_jpeg.h"
#include "reg_def.h"

typedef enum{
	IDX_A_BUF = 0,
	IDX_B_BUF,	
}WriteBufIdx_e;

UINT8 IntrSpeedyCaptureReadVlcSize(UINT32 *vlcSize);
void IntrSpeedyCaptureVlcBufSwitch(UINT32 addr,UINT8 *bufIdx);



#endif