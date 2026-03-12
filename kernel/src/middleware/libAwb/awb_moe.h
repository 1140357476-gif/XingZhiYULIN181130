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
 * @file      File name
 * @brief     Brief
 * @author   wangjg
 * @since     2010-01-01
 * @date      2010-04-30
*/

#ifndef _AWB_MOE_H__
#define _AWB_MOE_H__
/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "sp1k_awb_api.h" 
#include "hal_moe.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define AWB_ADDR_MOE_rect_left 		 AWB_ADDR_Tmp0
#define AWB_ADDR_MOE_rect_right 	 AWB_ADDR_Tmp2
#define AWB_ADDR_MOE_rect_down 	 	 AWB_ADDR_Tmp1
#define AWB_ADDR_MOE_rect_up 		 AWB_ADDR_Tmp3
#define AWB_ADDR_MOE_rect_r 		 AWB_ADDR_Tmp4
#define AWB_ADDR_MOE_rect_b 		 AWB_ADDR_Tmp5
#define AWB_ADDR_MOE_rect 			 AWB_ADDR_Tmp6
#define AWB_ADDR_MOE_rect_wp 		 AWB_ADDR_Tmp7	
#define AWB_ADDR_MOE_rect_sum		(AWB_ADDR_Out+60) 

//slop_low
#define AWB_ADDR_MOE_slop_dx  AWB_ADDR_Tmp0
#define AWB_ADDR_MOE_slop_dy  AWB_ADDR_Tmp2
#define AWB_ADDR_MOE_slop_k  AWB_ADDR_Tmp4//8.8
#define AWB_ADDR_MOE_slop_cmp_k0  AWB_ADDR_Tmp0
#define AWB_ADDR_MOE_slop_cmp_k1  AWB_ADDR_Tmp1
#define AWB_ADDR_MOE_slop_dx_flag  AWB_ADDR_Tmp2
#define AWB_ADDR_MOE_slop_dy_flag  AWB_ADDR_Tmp3
#define AWB_ADDR_MOE_slop_k0dx  AWB_ADDR_Tmp4
#define AWB_ADDR_MOE_slop_k1dy  AWB_ADDR_Tmp5
#define AWB_ADDR_MOE_slop_left_down  AWB_ADDR_Tmp0;
#define AWB_ADDR_MOE_slop_and   AWB_ADDR_Tmp1

#define AWB_ADDR_MOE_slop_low   AWB_ADDR_Tmp6

#define AWB_ADDR_MOE_slop_high   AWB_ADDR_Tmp7
//slop
#define AWB_ADDR_MOE_slop   AWB_ADDR_Tmp0
//spot

//dis
#define AWB_ADDR_MOE_dis_rgain_c0  AWB_ADDR_Tmp0
#define AWB_ADDR_MOE_dis_sign  AWB_ADDR_Tmp6
//spot	

#define AWB_ADDR_MOE_spot_0  AWB_ADDR_Tmp0
#define AWB_ADDR_MOE_spot_1  AWB_ADDR_Tmp1
#define AWB_ADDR_MOE_spot_2  AWB_ADDR_Tmp2
#define AWB_ADDR_MOE_spot_3  AWB_ADDR_Tmp3	
#define AWB_ADDR_MOE_spot_4  AWB_ADDR_Tmp4
#define AWB_ADDR_MOE_spot_5  AWB_ADDR_Tmp5
//spot sum
#define AWB_ADDR_MOE_spot_rgain_wp   AWB_ADDR_Tmp6
#define AWB_ADDR_MOE_spot_bgain_wp   AWB_ADDR_Tmp8
#define AWB_ADDR_MOE_spot_sum   AWB_ADDR_Out 
#define AWB_ADDR_MOE_offset_sum	(AWB_ADDR_Out+AWB_SUM_NUM*12ul)
#define AWB_ADDR_MOE_all_sum (AWB_ADDR_MOE_offset_sum+20)
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define moeCpWrite(q,cp) q.src20=(UINT8)(cp);q.src21=(cp)>>8
#define moeCpRead(q) q.src20+((UINT16)q.src21<<8) 

#define printfen 1
#define printf0(str)
#define printf1(str,v0)
#define printf2(str,v0,v1)
#define printf3(str,v0,v1,v2)
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

#if 0
typedef struct
{	
	UINT32 queue[AWB_SCENE_NUM];
	UINT32 wp;
	UINT32 samp;
	UINT32 out;
	//temp
	UINT32 tmp[10]; 
	//gain
	UINT32 rgain;
	UINT32 bgain;
	
	//rect
	UINT32 rect_left;
	UINT32 rect_right;
	UINT32 rect_down;
	UINT32 rect_up;
	UINT32 rect_r;
	UINT32 rect_b;
	UINT32 rect;
	UINT32 rect_wp;	
	UINT32 rect_sum_wp;	
	UINT32 rect_sum_r;
	UINT32 rect_sum_b;

	//slop_low
	UINT32 slop_dx;
	UINT32 slop_dy;
	UINT32 slop_dx_flag;
	UINT32 slop_dy_flag;
	UINT32 slop_k;//8.8
	UINT32 slop_cmp_k0;
	UINT32 slop_cmp_k1;
	UINT32 slop_k0dx;
	UINT32 slop_k1dy;
	UINT32 slop_and;
	UINT32 slop_left_down;
	UINT32 slop_low;
	//slop_high
	UINT32 slop_high;
	//slop
	UINT32 slop;

	//dis
	UINT32 dis_rgain_c0;
	UINT32 dis_sign;
	//spot	
	UINT32 spot_0;
	UINT32 spot_1;
	UINT32 spot_2;
	UINT32 spot_3;	
	UINT32 spot_4;

	//spot sum
	UINT32 spotSum;

	UINT32 spot_rgain_wp;
	UINT32 spot_bgain_wp;

}awb_moe_buf_t;
#endif

#if 0
typedef struct
{
	moe_queue_t thr;
	moe_queue_t wp_2;
	moe_queue_t wp;
}awb_moe_thr_t;
typedef struct
{
	moe_queue_t sum_r;
	moe_queue_t sum_gr;
	moe_queue_t sum_b;	
	moe_queue_t sum_gb;
	moe_queue_t sum_wp;
}awb_moe_ob_t;
typedef struct //8 +12
{
	moe_queue_t low8bit;
	moe_queue_t high8bit;
	awb_moe_thr_t thrLow;
	awb_moe_thr_t thrHigh;	
	moe_queue_t thr;
	
	awb_moe_ob_t ob;
	
	moe_queue_t r;
	moe_queue_t b;
	moe_queue_t g;
	moe_queue_t wp;
	//moe_queue_t wpThr;
	moe_queue_t rg;
	moe_queue_t bg;

}awb_moe_raw_t;

#else
/*
typedef struct //4// 4
{
	moe_queue_t low;
	moe_queue_t high;
	moe_queue_t thr;
	moe_queue_t wp;
}awb_moe_thr_t;
*/
typedef struct //4// 4
{
	moe_queue_t low;
	moe_queue_t high;
	moe_queue_t thr3;
	moe_queue_t thr2;
	moe_queue_t thr;
	moe_queue_t wp;
}awb_moe_thr_t;
typedef struct
{
	//moe_queue_t thr_r;
	//moe_queue_t thr_b;
	//moe_queue_t thr_gr;
	//moe_queue_t thr_gb;
	moe_queue_t thr;
	moe_queue_t wp_2;
	moe_queue_t wp;
	moe_queue_t sum_r;
	moe_queue_t sum_gr;
	moe_queue_t sum_b;	
	moe_queue_t sum_gb;
	moe_queue_t sum_wp;
}awb_moe_offset_t;
typedef struct //8 +12
{
	moe_queue_t low8bit;
	moe_queue_t high8bit;
	awb_moe_offset_t offset;
	moe_queue_t r;
	moe_queue_t b;
	moe_queue_t g;
	moe_queue_t wp;
	awb_moe_thr_t thr;
	//awb_moe_thr_t rthr;
	//awb_moe_thr_t bthr;
	//awb_moe_thr_t gthr;
	moe_queue_t rg;
	moe_queue_t bg;
/*
	moe_queue_t wpSum;
	moe_queue_t rgSum;
	moe_queue_t bgSum;
*/
}awb_moe_raw_t;
#endif
typedef struct 
{
	moe_queue_t slop_dx;
	moe_queue_t slop_dy;	
	moe_queue_t slop_k;//8.8
	moe_queue_t slop_cmp_k0;
	moe_queue_t slop_cmp_k1;
	moe_queue_t slop_dx_flag;
	moe_queue_t slop_dy_flag;
	//moe_queue_t slop_k0dx;
	//moe_queue_t slop_k1dy;	
	moe_queue_t slop_k0dx_k1dy;
	moe_queue_t slop_left_down;
	moe_queue_t slop_and;
	moe_queue_t slop_low;
}awb_moe_slop_t;
typedef struct
{
	moe_queue_t rect_sum_wp;	
	moe_queue_t rect_sum_r;
	moe_queue_t rect_sum_b;
}awb_moe_sum_t;
typedef struct
{
	moe_queue_t rect_left;
	moe_queue_t rect_right;
	moe_queue_t rect_down;
	moe_queue_t rect_up;
	moe_queue_t rect_r;
	moe_queue_t rect_b;
	moe_queue_t rect;
	moe_queue_t rect_wp;	

}awb_moe_rect_t;

typedef struct
{

	moe_queue_t rect_left;
	moe_queue_t rect_right;
	moe_queue_t rect_down;
	moe_queue_t rect_up;
	moe_queue_t rect_2;
	//moe_queue_t rect_r;
	//moe_queue_t rect_b;
	moe_queue_t rect;
	moe_queue_t rect_wp;	
	moe_queue_t rect_sum_wp;	
	moe_queue_t rect_sum_r;
	moe_queue_t rect_sum_b;
	
}awb_moe_rect_sum_t;
typedef struct
{
	//slop
	moe_queue_t slop;
	//spot
	moe_queue_t spot_wp;
	//dis
	//moe_queue_t dis_rgain_c0;
	moe_queue_t dis_sign;
	//spot	
	moe_queue_t spot_0;
	moe_queue_t spot_1;
	moe_queue_t spot_2;
	moe_queue_t spot_3;	
	moe_queue_t spot_4;
	moe_queue_t spot_5;
	
	//spot sum
	moe_queue_t spot0_sum_wp;	
	moe_queue_t spot1_sum_wp;
	moe_queue_t spot2_sum_wp;
	moe_queue_t spot3_sum_wp;
	moe_queue_t spot4_sum_wp;
	moe_queue_t spot5_sum_wp;
	
	// gain wp
	moe_queue_t spot_rgain_wp;
	moe_queue_t spot_bgain_wp;

	moe_queue_t spot0_sum_r;
	moe_queue_t spot1_sum_r;
	moe_queue_t spot2_sum_r;
	moe_queue_t spot3_sum_r;
	moe_queue_t spot4_sum_r;
	moe_queue_t spot5_sum_r;

	moe_queue_t spot0_sum_b;
	moe_queue_t spot1_sum_b;
	moe_queue_t spot2_sum_b;
	moe_queue_t spot3_sum_b;
	moe_queue_t spot4_sum_b;
	moe_queue_t spot5_sum_b;
}awb_moe_spot_t;
typedef struct
{
	//gain
	awb_moe_raw_t raw;//20
	//rect
	awb_moe_rect_t rectSward;//sward
	awb_moe_rect_sum_t rectSky;//sky
	awb_moe_rect_sum_t rectOffice;//office
	awb_moe_rect_sum_t rectFlash;//flash
	awb_moe_rect_sum_t rectCalib;//calib
	awb_moe_rect_t rect;//8
		
	//slop_low
	awb_moe_slop_t slopL;
	//slop_high
	awb_moe_slop_t slopH;
	
	awb_moe_spot_t spot;
}awb_moe_normal_t;
typedef struct
{
	//gain
	awb_moe_raw_t raw;
	//rect
	awb_moe_rect_t rectSward;//sward
	awb_moe_rect_sum_t rectSky;//sky
	awb_moe_rect_sum_t rectOffice;//office
	awb_moe_rect_sum_t rectFlash;//flash
	awb_moe_rect_sum_t rectCalib;//calib
	awb_moe_rect_t rect;
		
	//slop_low
	awb_moe_slop_t slopL;
	//slop_high
	awb_moe_slop_t slopH;
	
	awb_moe_spot_t spot;
}awb_moe_outdoor_t;
typedef struct
{
	//gain
	awb_moe_raw_t raw;
	//rect
	awb_moe_rect_sum_t rectsum0;
	
}awb_moe_rectone_t; 

typedef struct
{
	awb_moe_normal_t normal;
	awb_moe_outdoor_t outdoor;
	awb_moe_rectone_t flash;
	//awb_moe_rectone_t calibration;
}awb_moe_queue_t;

typedef struct
{
	UINT32 sum;
	UINT16 max;
	UINT16 min;
}awb_sum_struct_t;
typedef struct
{
	awb_sum_struct_t wp;
	awb_sum_struct_t rg;
	awb_sum_struct_t bg;
}awb_sum_spot_t;

#ifdef AWB_MOE_NEW
typedef struct
{
	moe_queue_t spot;
	moe_queue_t spot_sum_wp;
	moe_queue_t spot_sum_rg;
	moe_queue_t spot_sum_bg;
}awb_moe_spot_sum_t;
typedef struct
{
	moe_queue_t slop;
	moe_queue_t slop_wp;
	moe_queue_t dis_sign;

	moe_queue_t rgain_wp;
	moe_queue_t bgain_wp;
	
	awb_moe_spot_sum_t spots_sum[9];
}awb_moe_spots_t;
typedef struct
{
	awb_moe_rect_t rect;
	awb_moe_slop_t slop0;
	awb_moe_slop_t slop1;
	awb_moe_spots_t spots;
}awb_moe_indoor0_t;
typedef struct
{
	//gain
	awb_moe_raw_t raw;//20
	//rect
	//awb_moe_rect_t rectSward;//sward
	//awb_moe_rect_sum_t rectSky;//sky
	awb_moe_rect_sum_t rectOffice;//office

	awb_moe_indoor0_t spots;
}awb_moe_indoor_t;
typedef struct
{
	moe_queue_t slop_dx;
	moe_queue_t slop_dy;	
	moe_queue_t slop_k;//8.8
	moe_queue_t slop_cmp_k0;
	moe_queue_t slop_cmp_k1;
	moe_queue_t slop_up;
	
	moe_queue_t rect_left;
	moe_queue_t rect_right;
	moe_queue_t rect_down;
	//moe_queue_t rect_up;
	moe_queue_t rect_2;
	//moe_queue_t rect_r;
	//moe_queue_t rect_b;
	moe_queue_t rect;

	moe_queue_t rgain_wp;
	moe_queue_t bgain_wp;
	
	awb_moe_spot_sum_t spots_sum[3];
}awb_moe_outdoor0_t;
typedef struct
{
	//gain
	awb_moe_raw_t raw;
	//rect
	awb_moe_rect_t rectSward;//sward
	//awb_moe_rect_sum_t rectSky;//sky
	awb_moe_rect_sum_t rectOffice;//office
	
	awb_moe_outdoor0_t spots;
}awb_moe_outdoor_t;
#endif
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/




#endif  /* __TEMPLATE_HDR_H__ */
