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
 * @file		panorama_def.h
 * @brief		panorama defibe module header file.
 * @author		LinJieCheng
 * @since		2013-02-19
 * @date			2013-02-19
 */
#ifndef __PANORAMA_DEF_H__
#define __PANORAMA_DEF_H__
#include "knl_func_opt.h"
//#include "sp1k_panorama_api.h"

#if FUNC_KNL_PANORAMA
#define PANORAMA_EN	1
#else
#define PANORAMA_EN	0
#endif
/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
#define PANORAMA_BUF_MAX_IDX	30

/**************************************************************************
 *                              D A T A   T Y P E S
 **************************************************************************/
#define ABS(a)			(((a) >= 0) ? (a) : -(a))
#define MAX(a, b)		(((a) > (b)) ? (a) : (b))


#define PANORAMA_FRAME_TMP_WSIZE (UINT32)(G_PanoramaInf.frameTmp.hSize*G_PanoramaInf.frameTmp.vSize)
#define PANORAMA_FRAME_MV_THD	10
#define SWAP_HV_EN	0

#define HALF_PIXEL_FAST_THD (SINT16)(16)
#define HALF_PIXEL_SLOW_THD (SINT16)(-16)


#define ROUND_DOWN_TO_DIVISIBLE(num,div) \
	( (UINT32)(num) & -(UINT32)(div) )
#define ROUND_UP_TO_DIVISIBLE(num,div) \
	ROUND_DOWN_TO_DIVISIBLE( (UINT32)(num) + (div) - 1, div )


#define GPRM_OFFSET_2N_CLAMP(val) \
	( (((UINT32)val) + 1) & 0xFFFFFFFE )

#define PANORAMA_DEFAULT_STITCH_PIXEL	80
#define PANORAMA_BLENDING_WIDTH			16

#if 1
#define STD_REF_WIDTH		3344

#define STD_FINAL_WIDTH_240		1440
#define STD_FINAL_WIDTH_480		2880
#define STD_FINAL_WIDTH_720		4768
#define STD_FINAL_WIDTH_1080	7152

#define STD_FINAL_HEIGHT_240	208
#define STD_FINAL_HEIGHT_480	400
#define STD_FINAL_HEIGHT_720	3280
#define STD_FINAL_HEIGHT_1080	4912
#endif

#define X_OFFSET	0
#define EXTEND		0


/**************************************************************************
 *                              D E B U G
 **************************************************************************/
#define DBG_DRAMDMA_ME_BY_FRAME		0
#if (DBG_DRAMDMA_ME_BY_FRAME == 1)
#define DBG_SAVE_ME_BY_FRAME	0
#endif

#define DBG_DRAMW_GME_BY_FRAME	1//panorama always turn on
#if (DBG_DRAMW_GME_BY_FRAME == 1)
#define DBG_SHOW_GME_BY_FRAME	0
#define DBG_SAVE_GME_BY_FRAME	1
#endif

#define DBG_DRAMDMA_SRC_YUV_BY_FRAME	1//panorama always turn on
#if (DBG_DRAMDMA_SRC_YUV_BY_FRAME == 1)
#define DBG_SAVE_SRC_YUV_BY_FRAME	1
#endif

#define DBG_SAVE_DST_YUV			1




/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
	

#endif  /* __PANORAMA_DEF_H__ */

