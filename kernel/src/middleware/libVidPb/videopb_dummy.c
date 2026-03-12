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
 * @file	  video_pb_dummy.c
 * @brief	  dummy API for video pb module
 * @author	  xianxin
 * @since	  2011-01-07
 * @date	  2011-01-07
*/

/**************************************************************************
 *						  H E A D E R	F I L E S
 **************************************************************************/
#include "general.h"
#include "dbg_file_list.h"
#include "video_pb.h"
#include "func_ptr.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define __FILE	__FILE_ID_VIDEOPB_DUMMY__
#define VDPB_DUMMY_DEBUG()	{printf("DUMMY F(%x), L(%u)\n", __FILE, __LINE__);}	

#if (FUNC_KNL_VDPB_LIB_EN == 0)
/**************************************************************************
 *						   G L O B A L	  D A T A						  *
 **************************************************************************/
UINT32 aviFileLength = 0;
UINT8 G_VPBupSmpflag = 0;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
//video_pb.c
UINT32 videoPbAviLengthGet(void)
{
	VDPB_DUMMY_DEBUG();
	return 0;
}
void videoPbStart(UINT16 ID)
{
	VDPB_DUMMY_DEBUG();
	ID=ID;
	pFunc_VideoPb();
}
void videoPbStop(void)
{
	VDPB_DUMMY_DEBUG();
}
void videoPbPause(void)
{
	VDPB_DUMMY_DEBUG();
}
void videoPbResume(void)
{
	VDPB_DUMMY_DEBUG();
}
UINT32 videoPbReadDword(UINT32 dramAddr)
{
	VDPB_DUMMY_DEBUG();
	return 0;
	dramAddr = 0;
}
void videoPb(void) 
{
	VDPB_DUMMY_DEBUG();
}
UINT8 videoPbStatusGet(void)
{
	VDPB_DUMMY_DEBUG();
	return 0;
}
UINT8 videoPbSpeedGet(void)
{
	VDPB_DUMMY_DEBUG();
	return 0;
}
void videoPbSizeGet(UINT16 *phSize, UINT16 *pvSize)
{
	VDPB_DUMMY_DEBUG();
	phSize = phSize;
	pvSize = pvSize;
}

//video_pbbf.c
void videoPbFWSpeedSet(UINT8 speed)
{
	VDPB_DUMMY_DEBUG();
	speed = speed;
}
void videoPbRWSpeedSet(UINT8 speed)
{
	VDPB_DUMMY_DEBUG();
	speed = speed;
}
void videoPbSlowFWSpeedSet(UINT8 speed)
{
	VDPB_DUMMY_DEBUG();
	speed = speed;
}

#endif

