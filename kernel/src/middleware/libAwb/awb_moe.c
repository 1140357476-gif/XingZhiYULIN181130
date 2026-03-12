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
 * @author    Author's name
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "awb_func.h"


#if AWB_ENABLE
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
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
#if 0
void awbMoeThr(awb_moe_thr_t *pt,UINT32 out,UINT8 thr) 
{
	moe_cmd_t cmd;
	UINT16 cnt = AWB_SAMP_CNT;
	
	HAL_MoeDefCmd(&cmd);	
	cmd.equation = MOE_EQUATION_Subtract;
	cmd.func = 1;
	cmd.exchange = 1;
	cmd.ROIWidth = cnt*4;
	//cmd.min = 0;
	cmd.max = 1;
	cmd.src1 = AWB_ADDR_Tmp0;//+512*cr;
	cmd.cp = thr;//thr[0];
	cmd.dst = AWB_ADDR_Tmp6;//[6];
	HAL_MoeConvertToQueue(&cmd, &(pt->thr)); // thr	
	
	cmd.equation = MOE_EQUATION_Min;
	cmd.func = 0;
	cmd.exchange = 0;
	cmd.ROIWidth = cnt+cnt;
	cmd.src1 = AWB_ADDR_Tmp6;//AWB_ADDR_RGain;
	cmd.src2 = AWB_ADDR_Tmp8;//AWB_ADDR_RGain;
	cmd.dst = AWB_ADDR_Tmp6;//[0];	//tmp[0~2].r
	HAL_MoeConvertToQueue(&cmd, &(pt->wp_2)); 
	cmd.ROIWidth = cnt;
	cmd.src1 = AWB_ADDR_Tmp6;//AWB_ADDR_RGain;
	cmd.src2 = AWB_ADDR_Tmp7;//AWB_ADDR_RGain;
	cmd.dst = out;//[0];	//tmp[0~2].r
	HAL_MoeConvertToQueue(&cmd, &(pt->wp)); 
}
void awbMoeOb(awb_moe_ob_t*pt,UINT32 addrThr,UINT8 cr,UINT8 cgr,UINT8 cb,UINT8 cgb)
{
	moe_cmd_t cmd;
	UINT16 cnt = AWB_SAMP_CNT;	

	HAL_MoeDefCmd(&cmd);
	cmd.equation = MOE_EQUATION_Summation;
	cmd.sum = 0;
	cmd.max = 0x7fffffff;
	cmd.src1 = addrThr;
	cmd.src1Shift = 4;
	cmd.src2 = AWB_ADDR_Tmp0+512*cr;
	cmd.src2Shift = 4;	
	cmd.dst = AWB_ADDR_MOE_offset_sum;
	HAL_MoeConvertToQueue(&cmd, &(pt->sum_r));
	cmd.src2 = AWB_ADDR_Tmp0+512*cgr;
	cmd.dst = AWB_ADDR_MOE_offset_sum+4;
	HAL_MoeConvertToQueue(&cmd, &(pt->sum_gr));
	cmd.src2 = AWB_ADDR_Tmp0+512*cb;
	cmd.dst = AWB_ADDR_MOE_offset_sum+8;
	HAL_MoeConvertToQueue(&cmd, &(pt->sum_b));
	cmd.src2 = AWB_ADDR_Tmp0+512*cgb;
	cmd.dst = AWB_ADDR_MOE_offset_sum+12;
	HAL_MoeConvertToQueue(&cmd, &(pt->sum_gb));
	cmd.func=1;
	cmd.src1Shift = 0;	
	cmd.cp = 1;
	cmd.src2Shift = 0;	
	cmd.dst = AWB_ADDR_MOE_offset_sum+16;
	HAL_MoeConvertToQueue(&cmd, &(pt->sum_wp));
}
void awbMoeRaw(awb_moe_raw_t*pt)//awb_moe_raw_t *pt, awb_addr_t *ad)//ok
{            
	moe_cmd_t cmd;
	UINT16 cnt = AWB_SAMP_CNT;	

	UINT8  cr,cgr,cgb,cb;

	UINT8 fmt = 0x03;//XBYTE[0x2110];
	//UINT8 fmt=0x00;
	//fmt &=0x11;
	fmt &= 0x03;
	/*
	0   BAYER_GRBG 
	1   BAYER_RGGB 
	2   BAYER_BGGR 
	3   BAYER_GBRG 
	*/

	/*
			copy to:		moe to:		R Gr Gb B
	    
	0:GRBG	Gr  	R		R Gb Gr B 	0 2  1  3
			B	Gb
			
	1:RGGB	R  	Gr		Gr B R Gb  	2 0  3  1
			Gb	B
	    
	2:BGGR	B  	Gb		Gb R	 B Gr 	1 3  0  2
			Gr	R    	

	1:GBRG	Gb  	B		B Gr	Gb R 	3 1  2 0
			R	Gr
	*/
	if(fmt== 0x00) {//GRBG
		//cr = 1;cgr = 0;cgb = 33;cb = 32;
		cr = 0;cgr = 2;cgb = 1;cb = 3;
		//cr = 2;cgr = 0;cgb = 3;cb = 1;
	}
	else if(fmt== 0x01) {//RGGB
		//cr = 0;cgr = 1;cgb = 32;cb = 33;
		cr = 2;cgr = 0;cgb = 3;cb = 1;
		//cr = 0;cgr = 2;cgb = 1;cb = 3;
	}
	else if(fmt == 0x02/*0x10*/) {//BGGR
		//cr = 33;cgr = 32;cgb = 1;cb = 0;
		cr = 1;cgr = 3;cgb = 0;cb = 2;
		//cr = 3;cgr = 1;cgb = 2;cb = 0;
	}
	
	else if(fmt == 0x03/*0x11*/) {//GBRG
		//cr = 32;cgr = 33;cgb = 0;cb = 1;
		cr = 3;cgr = 1;cgb = 2;cb = 0;
		//cr = 1;cgr = 3;cgb = 0;cb = 2;
	}

	
	//r gr 
	//gb b
	HAL_MoeDefCmd(&cmd);
	cmd.equation = MOE_EQUATION_Add;
	cmd.func = 1;
	cmd.ROIWidth = (UINT16)(AWB_SAMP_CNT + AWB_SAMP_CNT);
	//cmd.min = 0;
	//cmd.max = 1;
	cmd.src1Bit = 1;
	cmd.dstShift = 8;
	cmd.cp = 0;
	cmd.src1 = AWB_ADDR_Wnd1;//AWB_ADDR_RGain;
	cmd.dst = AWB_ADDR_Tmp0;//[0];	//tmp[0~2].r
	HAL_MoeConvertToQueue(&cmd, &(pt->low8bit)); // gb  b
	cmd.src1Shift = 8;
	cmd.dstShift = 8;
	cmd.dst = AWB_ADDR_Tmp2;//[2];	//tmp[0~2].r
//cmd.CmdQueueEnd = 1;
	HAL_MoeConvertToQueue(&cmd, &(pt->high8bit)); // r  gb

	awbMoeThr(&(pt->thrLow),AWB_ADDR_Tmp4,16);
	awbMoeThr(&(pt->thrHigh),AWB_ADDR_Tmp5,250);
	
	HAL_MoeDefCmd(&cmd);
	cmd.equation = MOE_EQUATION_Subtract;
	cmd.max=1;
	cmd.min=0;
	cmd.ROIWidth = cnt;
	cmd.src1 = AWB_ADDR_Tmp5;//high
	cmd.src2 = AWB_ADDR_Tmp4;//low
	cmd.dst = AWB_ADDR_Tmp5;
	HAL_MoeConvertToQueue(&cmd, &(pt->thr)); 
	
	awbMoeOb(&(pt->ob),AWB_ADDR_Tmp4,cr,cgr,cb,cgb);

	//r  gr
	//gb b
	HAL_MoeDefCmd(&cmd);
	cmd.ROIWidth = AWB_SAMP_CNT;//copy
	cmd.func = 1;
	cmd.equation = MOE_EQUATION_Add;

	cmd.src1Bit = 0;
	cmd.src2Bit = 0;
	cmd.dstBit = 0;
	cmd.src1Shift = 0;
	cmd.src2Shift = 0;
	cmd.dstShift = 0;
	cmd.src1 = AWB_ADDR_Tmp0+512*cr;//[cr];// 2];
	cmd.cp = 0;
	cmd.dst = AWB_ADDR_R;//AWB_ADDR_Samp/*AWB_ADDR_MOE_samp*/;//tmp[4];	
	HAL_MoeConvertToQueue(&cmd, &(pt->r));
	cmd.src1 = AWB_ADDR_Tmp+512*cb;//[cb];// 1];
	cmd.dst = AWB_ADDR_B;//AWB_ADDR_Samp/*AWB_ADDR_MOE_samp*/+1024;//tmp[5];	
	HAL_MoeConvertToQueue(&cmd, &(pt->b));
	cmd.func = 0;
	cmd.src1 = AWB_ADDR_Tmp+512*cgr;//[cgr];// 0];
	cmd.src2 = AWB_ADDR_Tmp+512*cgb;//[cgb];// 3];
	cmd.dstShift = 1;
	cmd.dst = AWB_ADDR_G;//AWB_ADDR_Samp/*AWB_ADDR_MOE_samp*/+512;//tmp[4];	//g
	HAL_MoeConvertToQueue(&cmd, &(pt->g));
/*
	cmd.func = 1;
	cmd.max = 8;
	cmd.min = 1;
	cmd.dstShift = 5;
	cmd.src1 = AWB_ADDR_G;//AWB_ADDR_Samp+512;//AWB_ADDR_Tmp[4];//g          
	cmd.cp = 0;            
	cmd.dst = AWB_ADDR_Wp;//AWB_ADDR_MOE_wp;	//tmp[0~2].r
	HAL_MoeConvertToQueue(&cmd, &(pt->wp)); // gb  b
*/
	HAL_MoeDefCmd(&cmd);
	cmd.equation = MOE_EQUATION_Multiply;
	cmd.max = 8;
	cmd.min = 0;
	cmd.ROIWidth = cnt;
	//cmd.src1Shift = 4;
	cmd.src2Shift = 4;
	cmd.dstShift = 1;//5;
	cmd.src1 = AWB_ADDR_G;//G
	cmd.src2 = AWB_ADDR_Tmp5;//Thr
	cmd.dst = AWB_ADDR_Wp;
	HAL_MoeConvertToQueue(&cmd, &(pt->wp)); 

	//HAL_MoeDefCmd(&cmd);
	//cmd.clipSrc1En=0;
	//cmd.clipSrc2En=0;
	cmd.equation = MOE_EQUATION_Division;
	cmd.ROIWidth = AWB_SAMP_CNT;
	cmd.func = 0;
	cmd.max = 0x400;
	cmd.min = 0;
	cmd.src1Shift = 8;
	cmd.src2Shift = 6;
	cmd.dstShift = 0;
	cmd.dstBit = 1;
	cmd.src1 = AWB_ADDR_G;/*AWB_ADDR_MOE_samp+512*///tmp[4];//g
	cmd.src2 = AWB_ADDR_R;/*AWB_ADDR_MOE_samp*///tmp[cr];
	cmd.dst = AWB_ADDR_RGain;
	HAL_MoeConvertToQueue(&cmd, &(pt->rg));
	cmd.src2 = AWB_ADDR_B;/*AWB_ADDR_MOE_samp+1024*///tmp[cb];
	cmd.dst = AWB_ADDR_BGain;
//cmd.CmdQueueEnd = 1;
	HAL_MoeConvertToQueue(&cmd, &(pt->bg));

}//ok

#else
/**
 * @fn        void awbMoeThr(awb_moe_thr_t *pt, awb_moe_buf_t *ad, awb_moe_buf_t *ad,UINT32addr)
 * @brief     create jobqueue of samples threshold
 * @param     [out] pt	:job queue pointer
 * @param     [in] ad	:buf struct pointer
 * @param     [in] addr	:RGB data addr(Wold)
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
#if 0
void awbMoeThr(awb_moe_thr_t *pt,UINT32 addr)
{            
	moe_cmd_t cmd;// = new moe_cmd_t();
//printf("thr\n");
	HAL_MoeDefCmd(&cmd);
	cmd.equation = MOE_EQUATION_Subtract;
	cmd.func = 1;
	cmd.ROIWidth = AWB_SAMP_CNT;
	//cmd.min = 0;
	cmd.max = 1;
	cmd.src1 = addr;
	cmd.cp = 10;//thr[0];
	cmd.dst = AWB_ADDR_Tmp6;//[6];
	HAL_MoeConvertToQueue(&cmd, &(pt->low)); // left	
	//cmd.exchange = MOE_FUNC_Exchange;
	cmd.cp = 250;//thr[1];
	cmd.dst = AWB_ADDR_Tmp7;//[7];
	HAL_MoeConvertToQueue(&cmd, &(pt->high)); // right	
	cmd.func = 0;
	cmd.src1 = AWB_ADDR_Tmp6;//[6];
	cmd.src2 = AWB_ADDR_Tmp7;//[7];
	cmd.dst = AWB_ADDR_Tmp5;//[5];
	HAL_MoeConvertToQueue(&cmd, &(pt->thr));
	cmd.equation = MOE_EQUATION_Multiply;
	//cmd.func = 0;
	//cmd.exchange = 0;
	//cmd.CmdValid = 1;
	cmd.max = 8;
	cmd.src1Shift = 4;
	cmd.src2Shift = 4;
	cmd.src1 = AWB_ADDR_Tmp5;//[5];
	cmd.src2 = AWB_ADDR_Wp;//AWB_ADDR_MOE_wp;
	cmd.dst = AWB_ADDR_Wp;//AWB_ADDR_MOE_wp;
	HAL_MoeConvertToQueue(&cmd, &(pt->wp));
} 
#endif
void awbMoeThr(awb_moe_thr_t *pt,UINT32 addr)
{            
	moe_cmd_t cmd;// = new moe_cmd_t();
//printf("thr\n");
	HAL_MoeDefCmd(&cmd);
	cmd.equation = MOE_EQUATION_Subtract;
	cmd.func = 1;
	cmd.ROIWidth = AWB_SAMP_CNT*3;
	//cmd.min = 0;
	cmd.max = 1;
	cmd.src1 = addr;
	cmd.cp = 10;//thr[0];
	cmd.dst = AWB_ADDR_Tmp0;//[6];
	HAL_MoeConvertToQueue(&cmd, &(pt->low)); // left	
	//cmd.exchange = MOE_FUNC_Exchange;
	cmd.cp = 250;//thr[1];
	cmd.dst = AWB_ADDR_Tmp3;//[7];
	HAL_MoeConvertToQueue(&cmd, &(pt->high)); // right	
	cmd.func = 0;
	cmd.src1 = AWB_ADDR_Tmp0;//[6];
	cmd.src2 = AWB_ADDR_Tmp3;//[7];
	cmd.dst = AWB_ADDR_Tmp0;//[5];
	HAL_MoeConvertToQueue(&cmd, &(pt->thr3));
	cmd.equation = MOE_EQUATION_Min;
	cmd.ROIWidth = AWB_SAMP_CNT;
	cmd.src1 = AWB_ADDR_Tmp0;//[6];
	cmd.src2 = AWB_ADDR_Tmp1;//[7];
	cmd.dst = AWB_ADDR_Tmp0;//[5];
	HAL_MoeConvertToQueue(&cmd, &(pt->thr2));
	cmd.src2 = AWB_ADDR_Tmp2;//[7];
	HAL_MoeConvertToQueue(&cmd, &(pt->thr));
	cmd.equation = MOE_EQUATION_Multiply;
	//cmd.func = 0;
	//cmd.exchange = 0;
	//cmd.CmdValid = 1;
	cmd.max = 8;
	cmd.src1Shift = 4;
	cmd.src2Shift = 4;
	cmd.src1 = AWB_ADDR_Tmp0;//[5];
	cmd.src2 = AWB_ADDR_Wp;//AWB_ADDR_MOE_wp;
	cmd.dst = AWB_ADDR_Wp;//AWB_ADDR_MOE_wp;
	HAL_MoeConvertToQueue(&cmd, &(pt->wp));
}    
void awbMoeOffset(awb_moe_offset_t*ptCmd,UINT8 cr,UINT8 cgr,UINT8 cb,UINT8 cgb)
{
	moe_cmd_t cmd;
	awb_moe_offset_t *pt=ptCmd;//&(ptCmd->raw);
	//awb_moe_buf_t* ad=&moeAddr;
	UINT16 cnt = AWB_SAMP_CNT;	

	#if 0
	HAL_MoeDefCmd(&cmd);	
	cmd.equation = MOE_EQUATION_Subtract;
	cmd.func = 1;
	cmd.exchange = 1;
	cmd.ROIWidth = cnt;
	//cmd.min = 0;
	cmd.max = 1;
	cmd.src1 = AWB_ADDR_Tmp0+512*cr;
	cmd.cp = 10;//thr[0];
	cmd.dst = AWB_ADDR_Tmp6;//[6];
	HAL_MoeConvertToQueue(&cmd, &(pt->thr_r)); // cr	
	cmd.src1 = AWB_ADDR_Tmp0+512*cb;
	cmd.cp = 10;//thr[0];
	cmd.dst = AWB_ADDR_Tmp7;//[6];
	HAL_MoeConvertToQueue(&cmd, &(pt->thr_b)); // cb	
	cmd.src1 = AWB_ADDR_Tmp0+512*cgr;
	cmd.cp = 10;//thr[0];
	cmd.dst = AWB_ADDR_Tmp8;//[6];
	HAL_MoeConvertToQueue(&cmd, &(pt->thr_gr)); // cgr	
	cmd.src1 = AWB_ADDR_Tmp0+512*cgb;
	cmd.cp = 10;//thr[0];
	cmd.dst = AWB_ADDR_Tmp9;//[6];
	HAL_MoeConvertToQueue(&cmd, &(pt->thr_gb)); // cgb	
	#else
	HAL_MoeDefCmd(&cmd);	
	cmd.equation = MOE_EQUATION_Subtract;
	cmd.func = 1;
	cmd.exchange = 1;
	cmd.ROIWidth = cnt*4;
	//cmd.min = 0;
	cmd.max = 1;
	cmd.src1 = AWB_ADDR_Tmp0;//+512*cr;
	cmd.cp = 16;//thr[0];
	cmd.dst = AWB_ADDR_Tmp6;//[6];
	HAL_MoeConvertToQueue(&cmd, &(pt->thr)); // thr	
	#endif
	
	cmd.equation = MOE_EQUATION_Min;
	cmd.func = 0;
	cmd.exchange = 0;
	cmd.ROIWidth = cnt+cnt;
	cmd.src1 = AWB_ADDR_Tmp6;//AWB_ADDR_RGain;
	cmd.src2 = AWB_ADDR_Tmp8;//AWB_ADDR_RGain;
	cmd.dst = AWB_ADDR_Tmp6;//[0];	//tmp[0~2].r
	HAL_MoeConvertToQueue(&cmd, &(pt->wp_2)); 
	cmd.ROIWidth = cnt;
	cmd.src1 = AWB_ADDR_Tmp6;//AWB_ADDR_RGain;
	cmd.src2 = AWB_ADDR_Tmp7;//AWB_ADDR_RGain;
	cmd.dst = AWB_ADDR_Tmp6;//[0];	//tmp[0~2].r
	HAL_MoeConvertToQueue(&cmd, &(pt->wp)); 

	cmd.equation = MOE_EQUATION_Summation;
	cmd.sum = 0;
	cmd.max = 0x7fffffff;
	cmd.src1 = AWB_ADDR_Tmp6;
	cmd.src1Shift = 4;
	cmd.src2 = AWB_ADDR_Tmp0+512*cr;
	cmd.src2Shift = 4;	
	cmd.dst = AWB_ADDR_MOE_offset_sum;
	HAL_MoeConvertToQueue(&cmd, &(pt->sum_r));
	cmd.src2 = AWB_ADDR_Tmp0+512*cgr;
	cmd.dst = AWB_ADDR_MOE_offset_sum+4;
	HAL_MoeConvertToQueue(&cmd, &(pt->sum_gr));
	cmd.src2 = AWB_ADDR_Tmp0+512*cb;
	cmd.dst = AWB_ADDR_MOE_offset_sum+8;
	HAL_MoeConvertToQueue(&cmd, &(pt->sum_b));
	cmd.src2 = AWB_ADDR_Tmp0+512*cgb;
	cmd.dst = AWB_ADDR_MOE_offset_sum+12;
	HAL_MoeConvertToQueue(&cmd, &(pt->sum_gb));
	cmd.func=1;
	cmd.src1Shift = 0;	
	cmd.cp = 1;
	cmd.src2Shift = 0;	
	cmd.dst = AWB_ADDR_MOE_offset_sum+16;
	HAL_MoeConvertToQueue(&cmd, &(pt->sum_wp));
}
/**
 * @fn        void awbMoeRaw(awb_moe_raw_t* ptCmd, awb_moe_buf_t* ad)
 * @brief     awbMoeRaw
 * @param     [in] ptCmd
 * @param     [in] ad
 * @retval    NONE
 * @author    wangjg
 * @since     2010-6-19
*/
void awbMoeRaw(awb_moe_raw_t*ptCmd,UINT8 bayerPtn)//awb_moe_raw_t *pt, awb_addr_t *ad)//ok
{            
	moe_cmd_t cmd;
	awb_moe_raw_t *pt=ptCmd;//&(ptCmd->raw);
	//awb_moe_buf_t* ad=&moeAddr;
	UINT16 cnt = AWB_SAMP_CNT;	

	UINT8  cr,cgr,cgb,cb;

	UINT8 fmt = bayerPtn;//XBYTE[0x2110];
	//UINT8 fmt=0x00;
	//fmt &=0x11;
	fmt &= 0x03;
	/*
	0   BAYER_GRBG 
	1   BAYER_RGGB 
	2   BAYER_BGGR 
	3   BAYER_GBRG 
	*/

	/*
			copy to:		moe to:		R Gr Gb B
	    
	0:GRBG	Gr  	R		R Gb Gr B 	0 2  1  3
			B	Gb
			
	1:RGGB	R  	Gr		Gr B R Gb  	2 0  3  1
			Gb	B
	    
	2:BGGR	B  	Gb		Gb R	 B Gr 	1 3  0  2
			Gr	R    	

	1:GBRG	Gb  	B		B Gr	Gb R 	3 1  2 0
			R	Gr
	*/
	if(fmt== 0x00) {//GRBG
		//cr = 1;cgr = 0;cgb = 33;cb = 32;
		cr = 0;cgr = 2;cgb = 1;cb = 3;
		//cr = 2;cgr = 0;cgb = 3;cb = 1;
	}
	else if(fmt== 0x01) {//RGGB
		//cr = 0;cgr = 1;cgb = 32;cb = 33;
		cr = 2;cgr = 0;cgb = 3;cb = 1;
		//cr = 0;cgr = 2;cgb = 1;cb = 3;
	}
	else if(fmt == 0x02/*0x10*/) {//BGGR
		//cr = 33;cgr = 32;cgb = 1;cb = 0;
		cr = 1;cgr = 3;cgb = 0;cb = 2;
		//cr = 3;cgr = 1;cgb = 2;cb = 0;
	}
	
	else if(fmt == 0x03/*0x11*/) {//GBRG
		//cr = 32;cgr = 33;cgb = 0;cb = 1;
		cr = 3;cgr = 1;cgb = 2;cb = 0;
		//cr = 1;cgr = 3;cgb = 0;cb = 2;
	}

	
	//r gr 
	//gb b
	HAL_MoeDefCmd(&cmd);
	cmd.equation = MOE_EQUATION_Add;
	cmd.func = 1;
	cmd.ROIWidth = (UINT16)(AWB_SAMP_CNT + AWB_SAMP_CNT);
	//cmd.min = 0;
	//cmd.max = 1;
	cmd.max = 255;
	cmd.src1Bit = 1;
	cmd.dstShift = 8;
	cmd.cp = 0;
	cmd.src1 = AWB_ADDR_Wnd1;//AWB_ADDR_RGain;
	cmd.dst = AWB_ADDR_Tmp0;//[0];	//tmp[0~2].r
	HAL_MoeConvertToQueue(&cmd, &(pt->low8bit)); // gb  b
	cmd.src1Shift = 8;
	cmd.dstShift = 8;
	cmd.dst = AWB_ADDR_Tmp2;//[2];	//tmp[0~2].r
//cmd.CmdQueueEnd = 1;
	HAL_MoeConvertToQueue(&cmd, &(pt->high8bit)); // r  gb

	awbMoeOffset(&(pt->offset),cr,cgr,cb,cgb);

	//r  gr
	//gb b
	HAL_MoeDefCmd(&cmd);
	cmd.ROIWidth = AWB_SAMP_CNT;//copy
	cmd.func = 1;
	cmd.equation = MOE_EQUATION_Add;

	cmd.src1Bit = 0;
	cmd.src2Bit = 0;
	cmd.dstBit = 0;
	cmd.src1Shift = 0;
	cmd.src2Shift = 0;
	cmd.dstShift = 0;
	cmd.src1 = AWB_ADDR_Tmp0+512*cr;//[cr];// 2];
	cmd.cp = 0;
	cmd.dst = AWB_ADDR_R;//AWB_ADDR_Samp/*AWB_ADDR_MOE_samp*/;//tmp[4];	
	HAL_MoeConvertToQueue(&cmd, &(pt->r));
	cmd.src1 = AWB_ADDR_Tmp+512*cb;//[cb];// 1];
	cmd.dst = AWB_ADDR_B;//AWB_ADDR_Samp/*AWB_ADDR_MOE_samp*/+1024;//tmp[5];	
	HAL_MoeConvertToQueue(&cmd, &(pt->b));
	cmd.func = 0;
	cmd.src1 = AWB_ADDR_Tmp+512*cgr;//[cgr];// 0];
	cmd.src2 = AWB_ADDR_Tmp+512*cgb;//[cgb];// 3];
	cmd.dstShift = 1;
	cmd.dst = AWB_ADDR_G;//AWB_ADDR_Samp/*AWB_ADDR_MOE_samp*/+512;//tmp[4];	//g
	HAL_MoeConvertToQueue(&cmd, &(pt->g));
	//min effect before dstShift
	cmd.equation = MOE_EQUATION_Division;
	cmd.func = 1;
	cmd.max = 8;
	cmd.min = 1;
	cmd.src1Shift = 0;
	cmd.src2Shift = 6;
	cmd.dstShift = 0;
	cmd.src1 = AWB_ADDR_G;//AWB_ADDR_Samp/*AWB_ADDR_MOE_samp*/+512;//AWB_ADDR_Tmp[4];//g          
	cmd.cp = 32;            
	cmd.dst = AWB_ADDR_Wp;//AWB_ADDR_MOE_wp;	//tmp[0~2].r
	HAL_MoeConvertToQueue(&cmd, &(pt->wp)); // gb  b
	
	//thr
	awbMoeThr(&(pt->thr),  AWB_ADDR_R);//tmp[cr]);
	//awbMoeThr(&(pt->gthr),  AWB_ADDR_G);//tmp[4]);
	//awbMoeThr(&(pt->bthr),  AWB_ADDR_B);//tmp[cb]);
		

	//HAL_MoeDefCmd(&cmd);
	//cmd.clipSrc1En=0;
	//cmd.clipSrc2En=0;
	cmd.equation = MOE_EQUATION_Division;
	cmd.ROIWidth = AWB_SAMP_CNT;
	cmd.func = 0;
	cmd.max = 0x400;
	cmd.min = 0;
	cmd.src1Shift = 8;
	cmd.src2Shift = 6;
	cmd.dstShift = 0;
	cmd.dstBit = 1;
	cmd.src1 = AWB_ADDR_G;/*AWB_ADDR_MOE_samp+512*///tmp[4];//g
	cmd.src2 = AWB_ADDR_R;/*AWB_ADDR_MOE_samp*///tmp[cr];
	cmd.dst = AWB_ADDR_RGain;
	HAL_MoeConvertToQueue(&cmd, &(pt->rg));
	cmd.src2 = AWB_ADDR_B;/*AWB_ADDR_MOE_samp+1024*///tmp[cb];
	cmd.dst = AWB_ADDR_BGain;
//cmd.CmdQueueEnd = 1;
	HAL_MoeConvertToQueue(&cmd, &(pt->bg));
/*
	//sum
	cmd.func=MOE_FUNC_PointWithCommon;
	cmd.equation=MOE_EQUATION_Summation;
	cmd.sum=0;
	cmd.min=0;
	cmd.max=0xffffffff;
	cmd.exchange=0;
	cmd.src1Sign=0;
	cmd.src2Sign=0;
	cmd.src1Bit=1;
	cmd.src2Bit=0;
	cmd.dstBit=1;
	cmd.src1Shift=4;
	cmd.src2Shift=4;
	cmd.src1 = AWB_ADDR_Wp;
	cmd.cp = 1;		
	cmd.dst = AWB_ADDR_MOE_all_sum;
	HAL_MoeConvertToQueue(&cmd, &(pt->wpSum));
	cmd.func=MOE_FUNC_PointWithPoint;
	cmd.src2 = AWB_ADDR_RGain;
	cmd.dst = AWB_ADDR_MOE_all_sum+4;
	HAL_MoeConvertToQueue(&cmd, &(pt->rgSum));
	cmd.src2 = AWB_ADDR_RGain;
	cmd.dst = AWB_ADDR_MOE_all_sum+8;
	HAL_MoeConvertToQueue(&cmd, &(pt->bgSum));
	*/
}//ok
#endif
/**
 * @fn        void awbMoeSlop(awb_moe_slop_t* ptCmd, awb_moe_slop_t*ptCmd,awb_moe_buf_t* ad, ad,UINT8 flag)
 * @brief     create job queue of slop filter
 * @param     [out] ptCmd	:job queue pointer
 * @param     [in] ad		:buf struct pointer
 * @param     [in] flag		0: low threshold
						1: high threshold
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void awbMoeSlop(awb_moe_slop_t*ptCmd,UINT8 flag)
{
	moe_cmd_t cmd;
	awb_moe_slop_t *pt=ptCmd;//=&(ptCmd->slopL);
//	awb_moe_buf_t* ad=&moeAddr;
	UINT16 cnt = AWB_SAMP_CNT;	

	//if(flag==0)
	//	pt=&(ptCmd->slopL);
	//else
	//	pt=&(ptCmd->slopH);
	//gain 	
	HAL_MoeDefCmd(&cmd);
	//slop_low
#if 1
	cmd.equation = MOE_EQUATION_Absolute;
	cmd.func = MOE_FUNC_PointWithCommon;
	cmd.ROIWidth = cnt;
	cmd.min = 1;
	cmd.max = 255;
	cmd.src1Bit = 1;
	cmd.src2Bit= 1;
	cmd.dstBit= 1;
	cmd.src1Shift = 0;
	cmd.src2Shift = 0;
	cmd.src1 = AWB_ADDR_RGain;
//	cmd,0);
	cmd.dst = AWB_ADDR_MOE_slop_dx;
	HAL_MoeConvertToQueue(&cmd, &(pt->slop_dx)); //slop_dx
	cmd.src1 = AWB_ADDR_BGain;	
	cmd.dst =  AWB_ADDR_MOE_slop_dy;;
	HAL_MoeConvertToQueue(&cmd, &(pt->slop_dy));//slop_dy
	
	
	cmd.equation = MOE_EQUATION_Division;
	cmd.func = MOE_FUNC_PointWithPoint;
	cmd.min=0;
	cmd.max=0xffff;
	cmd.src1Shift = 8;
	cmd.src2Shift = 6;
	cmd.dstBit = 1;
	cmd.src1 = AWB_ADDR_MOE_slop_dy;//dy	
	cmd.src2 = AWB_ADDR_MOE_slop_dx;	
	cmd.dst =   AWB_ADDR_MOE_slop_k;	
	HAL_MoeConvertToQueue(&cmd, &(pt->slop_k));//slop_k//8.8	
	cmd.equation = MOE_EQUATION_Subtract;//k0>kk0
	cmd.func = MOE_FUNC_PointWithCommon;
	//cmd.exchange = 1;
	cmd.min = 0;
	cmd.max = 1;
	cmd.src1Shift = 0;
	cmd.src2Shift = 0;
	cmd.dstBit= 0;
	cmd.src1 = AWB_ADDR_MOE_slop_k;	
//	cmd,0);	
	cmd.dst = AWB_ADDR_MOE_slop_cmp_k0;	
	HAL_MoeConvertToQueue(&cmd, &(pt->slop_cmp_k0));//slop_cmp_k0		
	cmd.exchange = 1;//k0<kk1
//	cmd,0);
	cmd.dst = AWB_ADDR_MOE_slop_cmp_k1;	
	HAL_MoeConvertToQueue(&cmd, &(pt->slop_cmp_k1));//slop_cmp_k1
	
	//cmd.equation = MOE_EQUATION_Absolute;
	//cmd.func = MOE_FUNC_PointWithCommon;
	cmd.exchange = 0;	//flag
	//cmd.equation = MOE_EQUATION_Subtract;
	//cmd.max=1;
	//cmd.dstBit= 0;
	cmd.src1 = AWB_ADDR_RGain;
//	cmd,0);
	cmd.dst = AWB_ADDR_MOE_slop_dx_flag;	
	HAL_MoeConvertToQueue(&cmd, &(pt->slop_dx_flag));//slop_dx_flag
	cmd.src1 = AWB_ADDR_BGain;	
	cmd.dst =  AWB_ADDR_MOE_slop_dy_flag;
	HAL_MoeConvertToQueue(&cmd, &(pt->slop_dy_flag));//slop_dy_flag
	
	cmd.exchange=0;
	cmd.equation = MOE_EQUATION_Max;//or
	cmd.func = MOE_FUNC_PointWithPoint;
	cmd.ROIWidth = cnt*2;
	cmd.src1Bit=0;
	cmd.src2Bit=0;
	cmd.src1 = AWB_ADDR_MOE_slop_cmp_k0;	
	cmd.src2 = AWB_ADDR_MOE_slop_dx_flag;	
	cmd.dst = AWB_ADDR_MOE_slop_k0dx;
	HAL_MoeConvertToQueue(&cmd, &(pt->slop_k0dx_k1dy));//slop_k0dx	
	//cmd.src1 = AWB_ADDR_MOE_slop_cmp_k1;	
	//cmd.src2 = AWB_ADDR_MOE_slop_dy_flag;	
	//cmd.dst = AWB_ADDR_MOE_slop_k1dy;
	//HAL_MoeConvertToQueue(&cmd, &(pt->slop_k1dy));//slop_k1dy
	//cmd.equation = MOE_EQUATION_Add;//or
	//cmd.src1Shift = 0;
	//cmd.src2Shift = 0;
	cmd.ROIWidth = cnt;
	cmd.src1 = AWB_ADDR_MOE_slop_dx_flag;
	cmd.src2 = AWB_ADDR_MOE_slop_dy_flag;
	cmd.dst = AWB_ADDR_MOE_slop_left_down;
	HAL_MoeConvertToQueue(&cmd, &(pt->slop_left_down));//slop_left_dow
	
	cmd.equation = MOE_EQUATION_Min;//and
	
	//cmd.src1Shift = 4;
	//cmd.src2Shift = 4;
	cmd.src1 = AWB_ADDR_MOE_slop_k0dx;
	cmd.src2 = AWB_ADDR_MOE_slop_k1dy;
	cmd.dst = AWB_ADDR_MOE_slop_and;
	HAL_MoeConvertToQueue(&cmd, &(pt->slop_and));//slop_and

	//cmd.equation = MOE_EQUATION_Multiply;//and
	//cmd.src1Shift = 4;
	//cmd.src2Shift = 4;
	cmd.src1 = AWB_ADDR_MOE_slop_and;
	cmd.src2 = AWB_ADDR_MOE_slop_left_down;
	if(flag==0)
		cmd.dst = AWB_ADDR_MOE_slop_low;
	else
		cmd.dst = AWB_ADDR_MOE_slop_high;
	HAL_MoeConvertToQueue(&cmd, &(pt->slop_low));//slop_low
#endif
}
/**
 * @fn        void awbMoeSpot(awb_moe_spot_t* ptCmd, awb_moe_spot_t*ptCmd,awb_moe_buf_t* ad)
 * @brief     create job queue of sopt filter
 * @param     [out] ptCmd	:job queue pointer
 * @param     [in] ad		:buf struct pointer
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void awbMoeSpot(awb_moe_spot_t*ptCmd,UINT8 mode)
{
	moe_cmd_t cmd;
	awb_moe_spot_t *pt=ptCmd;
	UINT16 cnt = AWB_SAMP_CNT;	
	
	HAL_MoeDefCmd(&cmd);
	//slop
	cmd.equation = MOE_EQUATION_Subtract;//and
	cmd.ROIWidth = cnt;
	cmd.max = 1;
	//cmd.src1Shift = 0;
	//cmd.src2Shift = 0;
	cmd.src1 = AWB_ADDR_MOE_slop_low;
	cmd.src2 = AWB_ADDR_MOE_slop_high;
	cmd.dst = AWB_ADDR_MOE_slop;
	HAL_MoeConvertToQueue(&cmd, &(pt->slop));//slop

	cmd.equation = MOE_EQUATION_Multiply;//and
	cmd.src1Shift = 4;
	cmd.src2Shift = 4;
	cmd.max=32;
	cmd.src1 = AWB_ADDR_Wp;//AWB_ADDR_MOE_wp;
	cmd.src2 = AWB_ADDR_MOE_slop;//AWB_ADDR_MOE_spot;
	cmd.dst = AWB_ADDR_Wp;//AWB_ADDR_MOE_wp;//AWB_ADDR_MOE_spot_wp;
	HAL_MoeConvertToQueue(&cmd, &(pt->spot_wp));

	//dis
	/*cmd.func=MOE_FUNC_PointWithCommon;
	cmd.equation=MOE_EQUATION_Add;
	cmd.max=0x7fffffff;
	cmd.min=0;
	cmd.src1Bit = 1;
	cmd.src2Bit = 1;
	cmd.dstBit = 1;
	cmd.src1Shift = 0;
	cmd.src2Shift = 0;
	cmd.dstSign = 1;
	cmd.src1 = AWB_ADDR_RGain;
//	cmd,0);
	cmd.dst = AWB_ADDR_MOE_dis_rgain_c0;
//cmd.CmdQueueEnd=1;
	HAL_MoeConvertToQueue(&cmd, &(pt->dis_rgain_c0));*/
	
	cmd.func=MOE_FUNC_PointWithPoint;
	cmd.equation=MOE_EQUATION_Subtract;
	//cmd.exchange=0;
	cmd.min=-0x7fff;
	cmd.max=0x7fff;
	cmd.src1Bit = 1;
	cmd.src2Bit = 1;
	cmd.dstBit = 1;
	//cmd.src2Sign = 1;
	cmd.dstSign=1;
	cmd.src1Shift = 0;
	cmd.src2Shift = 0;
	cmd.src1 = AWB_ADDR_BGain;
	cmd.src2 = AWB_ADDR_RGain;//AWB_ADDR_MOE_dis_rgain_c0;
	cmd.dst = AWB_ADDR_MOE_dis_sign;
//cmd.CmdQueueEnd=1;
	HAL_MoeConvertToQueue(&cmd, &(pt->dis_sign));
	cmd.func=MOE_FUNC_PointWithCommon;
	cmd.equation=MOE_EQUATION_Subtract;
	cmd.exchange=0;
	cmd.max=1;
	cmd.min=0;
	cmd.src1Sign=1;
	cmd.src2Sign=1;
	cmd.dstSign=0;
	//cmd.src1Bit = 1;
	//cmd.src2Bit = 1;
	cmd.dstBit = 0;
	cmd.src1 = AWB_ADDR_MOE_dis_sign;
//	cmd,0);	
	cmd.dst = AWB_ADDR_MOE_spot_0;	
	HAL_MoeConvertToQueue(&cmd, &(pt->spot_0));
	cmd.dst = AWB_ADDR_MOE_spot_1;//AWB_ADDR_MOE_spot_40;	
	HAL_MoeConvertToQueue(&cmd, &(pt->spot_1));
	cmd.dst = AWB_ADDR_MOE_spot_2;//AWB_ADDR_MOE_spot_10;	
	HAL_MoeConvertToQueue(&cmd, &(pt->spot_2));
	if(mode)	cmd.src1 = AWB_ADDR_BGain;	
	cmd.dst = AWB_ADDR_MOE_spot_3;//AWB_ADDR_MOE_spot_50;	
	HAL_MoeConvertToQueue(&cmd, &(pt->spot_3));
	cmd.dst = AWB_ADDR_MOE_spot_4;//AWB_ADDR_MOE_spot_50;	
	HAL_MoeConvertToQueue(&cmd, &(pt->spot_4));
	
	cmd.exchange=1;
	cmd.dst = AWB_ADDR_MOE_spot_5;	
	HAL_MoeConvertToQueue(&cmd, &(pt->spot_5));


	//sum
	cmd.func=MOE_FUNC_PointWithPoint;
	cmd.equation=MOE_EQUATION_Summation;
	cmd.sum=0;
	cmd.min=0;
	cmd.max=0xffffffff;
	cmd.exchange=0;
	cmd.src1Sign=0;
	cmd.src2Sign=0;
	cmd.src1Bit=0;
	cmd.src2Bit=0;
	cmd.dstBit=1;
	cmd.src1Shift=4;
	cmd.src2Shift=4;
	cmd.src1 = AWB_ADDR_Wp;//AWB_ADDR_MOE_wp;//AWB_ADDR_MOE_spot_wp;
	cmd.src2 = AWB_ADDR_MOE_spot_0;		
	cmd.dst = AWB_ADDR_Out;//spot0_sum_wp;
	HAL_MoeConvertToQueue(&cmd, &(pt->spot0_sum_wp));
	cmd.src2 = AWB_ADDR_MOE_spot_1;
	cmd.dst = AWB_ADDR_Out+12;//spot1_sum_wp;
	HAL_MoeConvertToQueue(&cmd, &(pt->spot1_sum_wp));
	cmd.src2 = AWB_ADDR_MOE_spot_2;
	cmd.dst = AWB_ADDR_Out+24;//spot2_sum_wp;
	HAL_MoeConvertToQueue(&cmd, &(pt->spot2_sum_wp));
	cmd.src2 = AWB_ADDR_MOE_spot_3;
	cmd.dst = AWB_ADDR_Out+36;//spot4_sum_wp;
	HAL_MoeConvertToQueue(&cmd, &(pt->spot3_sum_wp));
	cmd.src2 = AWB_ADDR_MOE_spot_4;
	cmd.dst = AWB_ADDR_Out+48;//spot5_sum_wp;
	HAL_MoeConvertToQueue(&cmd, &(pt->spot4_sum_wp));
	cmd.src2 = AWB_ADDR_MOE_spot_5;
	cmd.dst = AWB_ADDR_Out+60;//spot5_sum_wp;
	HAL_MoeConvertToQueue(&cmd, &(pt->spot5_sum_wp));

	cmd.equation = MOE_EQUATION_Multiply;//and
	//cmd.src1Shift = 4;
	//cmd.src2Shift = 4;
	cmd.max = 10000;
	cmd.src1Bit = 1;
	cmd.src2Bit = 0;
	cmd.dstBit = 1;
	cmd.src1 = AWB_ADDR_RGain;
	cmd.src2 = AWB_ADDR_Wp;//AWB_ADDR_MOE_wp;//AWB_ADDR_MOE_spot_wp;
	cmd.dst = AWB_ADDR_MOE_spot_rgain_wp;
	HAL_MoeConvertToQueue(&cmd, &(pt->spot_rgain_wp));
	cmd.src1 = AWB_ADDR_BGain;
	cmd.dst = AWB_ADDR_MOE_spot_bgain_wp;
	HAL_MoeConvertToQueue(&cmd, &(pt->spot_bgain_wp));

	cmd.equation=MOE_EQUATION_Summation;
	cmd.max=0xffffffff;
	//cmd.src1Bit=1;
	//cmd.dstBit=1;
	cmd.src1 = AWB_ADDR_MOE_spot_rgain_wp;
	cmd.src2 = AWB_ADDR_MOE_spot_0;
	cmd.dst = AWB_ADDR_Out+4;//spot0_sum_r;
	HAL_MoeConvertToQueue(&cmd, &(pt->spot0_sum_r));
	cmd.src2 = AWB_ADDR_MOE_spot_1;
	cmd.dst = AWB_ADDR_Out+16;//spot1_sum_r;
	HAL_MoeConvertToQueue(&cmd, &(pt->spot1_sum_r));
	cmd.src2 = AWB_ADDR_MOE_spot_2;
	cmd.dst = AWB_ADDR_Out+28;//spot2_sum_r;
	HAL_MoeConvertToQueue(&cmd, &(pt->spot2_sum_r));
	cmd.src2 = AWB_ADDR_MOE_spot_3;
	cmd.dst = AWB_ADDR_Out+40;//spot4_sum_r;
	HAL_MoeConvertToQueue(&cmd, &(pt->spot3_sum_r));
	cmd.src2 = AWB_ADDR_MOE_spot_4;
	cmd.dst = AWB_ADDR_Out+52;//spot5_sum_r;
	HAL_MoeConvertToQueue(&cmd, &(pt->spot4_sum_r));
	cmd.src2 = AWB_ADDR_MOE_spot_5;
	cmd.dst = AWB_ADDR_Out+64;//spot5_sum_r;
	HAL_MoeConvertToQueue(&cmd, &(pt->spot5_sum_r));
	
	cmd.src1 = AWB_ADDR_MOE_spot_bgain_wp;
	cmd.src2 = AWB_ADDR_MOE_spot_0;
	cmd.dst = AWB_ADDR_Out+8;//spot0_sum_b;
	HAL_MoeConvertToQueue(&cmd, &(pt->spot0_sum_b));
	cmd.src2 = AWB_ADDR_MOE_spot_1;
	cmd.dst = AWB_ADDR_Out+20;//spot1_sum_b;
	HAL_MoeConvertToQueue(&cmd, &(pt->spot1_sum_b));
	cmd.src2 = AWB_ADDR_MOE_spot_2;
	cmd.dst = AWB_ADDR_Out+32;//spot2_sum_b;
	HAL_MoeConvertToQueue(&cmd, &(pt->spot2_sum_b));
	cmd.src2 = AWB_ADDR_MOE_spot_3;
	cmd.dst = AWB_ADDR_Out+44;//spot4_sum_b;
	HAL_MoeConvertToQueue(&cmd, &(pt->spot3_sum_b));
	cmd.src2 = AWB_ADDR_MOE_spot_4;
	cmd.dst = AWB_ADDR_Out+56;//spot5_sum_b;
	HAL_MoeConvertToQueue(&cmd, &(pt->spot4_sum_b));
	cmd.src2 = AWB_ADDR_MOE_spot_5;
	cmd.dst = AWB_ADDR_Out+68;//spot5_sum_b;
	HAL_MoeConvertToQueue(&cmd, &(pt->spot5_sum_b));
}

/**
 * @fn        void awbMoeRect(awb_moe_rect_t* ptCmd, awb_moe_buf_t* ad, UINT8 idx, ,UINT8 wp)
 * @brief     awbMoeRect
 * @param     [in] ptCmd	job queue	
 * @param     [in] ad		addr
 * @param     [in] idx		rect index
 * @param     [in] wp		is update to wp
 * @retval    NONE
 * @author    wangjg
 * @since     2010-6-19
*/void awbMoeRect(
	awb_moe_rect_t*ptCmd
	//,awb_moe_buf_t* ad
	,UINT8 idx
	,UINT8 mode // 0:wp 1:sum wp 2:sum gain 3:sum wp gain
	)
{
	moe_cmd_t cmd;
	awb_moe_rect_sum_t* pt=(awb_moe_rect_sum_t*)ptCmd;//&(ptCmd->rect);
	UINT16 cnt = AWB_SAMP_CNT;	
	UINT32 addr = AWB_ADDR_Out+ 12ul*(idx+6);
	HAL_MoeDefCmd(&cmd);
	//rect 

	cmd.equation = MOE_EQUATION_Subtract;
	cmd.func = MOE_FUNC_PointWithCommon;
	cmd.ROIWidth = cnt;
	cmd.min = 0;
	cmd.max = 1;	
	cmd.src1Bit = 1;
	cmd.src2Bit= 1;
	cmd.dstBit = 0;
	cmd.src1Shift=0;
	cmd.src2Shift =0;
//	cmd,0);	
	cmd.src1 = AWB_ADDR_RGain;
	cmd.dst =   AWB_ADDR_Tmp0;//AWB_ADDR_MOE_rect_left;	
	HAL_MoeConvertToQueue(&cmd, &(pt->rect_left)); // left	
	//cmd.exchange = MOE_FUNC_Exchange;
	cmd.dst =   AWB_ADDR_Tmp2;//AWB_ADDR_MOE_rect_right;
	HAL_MoeConvertToQueue(&cmd, &(pt->rect_right)); // right	
	//cmd.exchange = MOE_FUNC_NoExchange;
	cmd.src1 = AWB_ADDR_BGain;	
	cmd.dst =   AWB_ADDR_Tmp1;//AWB_ADDR_MOE_rect_down;
	HAL_MoeConvertToQueue(&cmd, &(pt->rect_down)); // down	
	//cmd.exchange = MOE_FUNC_Exchange;
	cmd.dst =   AWB_ADDR_Tmp3;//AWB_ADDR_MOE_rect_up;
	HAL_MoeConvertToQueue(&cmd, &(pt->rect_up)); // up	
	cmd.func = MOE_FUNC_PointWithPoint;
	cmd.ROIWidth = cnt*2;
	cmd.src1Bit = 0;
	cmd.src2Bit= 0;
	cmd.src1 = AWB_ADDR_Tmp0;//AWB_ADDR_MOE_rect_left;
	cmd.src2 = AWB_ADDR_Tmp2;//AWB_ADDR_MOE_rect_right;
	cmd.dst =  AWB_ADDR_Tmp4;//AWB_ADDR_MOE_rect_r;	
	HAL_MoeConvertToQueue(&cmd, &(pt->rect_2)); 	
	cmd.equation = MOE_EQUATION_Min;
	cmd.ROIWidth = cnt;
	cmd.src1 = AWB_ADDR_Tmp4;//AWB_ADDR_MOE_rect_r;
	cmd.src2 = AWB_ADDR_Tmp5;//AWB_ADDR_MOE_rect_b;
	cmd.dst =  AWB_ADDR_Tmp0;//AWB_ADDR_MOE_rect;
	HAL_MoeConvertToQueue(&cmd, &(pt->rect)); //rect
	
	cmd.equation = MOE_EQUATION_Multiply;//MOE_EQUATION_Multiply;//and
	cmd.src1Shift=4;
	cmd.src2Shift=4;
	cmd.max = 32;
	cmd.src1 = AWB_ADDR_Tmp0;//AWB_ADDR_MOE_rect;
	cmd.src2 = AWB_ADDR_Wp;//AWB_ADDR_MOE_wp;

	if(mode==0)
	{
		cmd.dst = AWB_ADDR_Wp;
		HAL_MoeConvertToQueue(&cmd, &(pt->rect_wp)); //rect
	}
	else if(mode==1)
	{
		cmd.equation = MOE_EQUATION_Summation;
		cmd.sum = 0;
		cmd.max = 0x7fffffff;
		cmd.dst = addr;
		HAL_MoeConvertToQueue(&cmd, &(pt->rect_wp)); //rect
	}
	else if(mode==2 || mode ==3)
	{
		UINT32 wp = (mode==2)?AWB_ADDR_Tmp2:AWB_ADDR_Wp;
		cmd.dst = wp;
		HAL_MoeConvertToQueue(&cmd, &(pt->rect_wp)); //rect
		cmd.equation = MOE_EQUATION_Summation;
		cmd.max = 0x7fffffff;
		cmd.dst =  addr;
		HAL_MoeConvertToQueue(&cmd, &(pt->rect_sum_wp)); 
		cmd.src2Bit = 1;		
		cmd.src1 = wp;		
		cmd.src2 = AWB_ADDR_RGain;
		cmd.dst =   addr+4;
		HAL_MoeConvertToQueue(&cmd, &(pt->rect_sum_r)); 
		cmd.src2 = AWB_ADDR_BGain;
		cmd.dst =   addr+8;
		HAL_MoeConvertToQueue(&cmd, &(pt->rect_sum_b)); 
	}
	
}
#if 0
void awbMoeRect(
	awb_moe_rect_t*ptCmd
	//,awb_moe_buf_t* ad
	,UINT8 idx
	,UINT8 wp
	)
{
	moe_cmd_t cmd;
	awb_moe_rect_t* pt=ptCmd;//&(ptCmd->rect);
	awb_moe_rect_sum_t* pt0=(awb_moe_rect_sum_t*)ptCmd;//&(ptCmd->rect);
	//awb_moe_buf_t* ad=&moeAddr;
	UINT16 cnt = AWB_SAMP_CNT;	

	HAL_MoeDefCmd(&cmd);
	//rect 

	cmd.equation = MOE_EQUATION_Subtract;
	cmd.func = MOE_FUNC_PointWithCommon;
	cmd.ROIWidth = cnt;
	cmd.min = 0;
	cmd.max = 1;	
	cmd.src1Bit = 1;
	cmd.src2Bit= 1;
	cmd.dstBit = 0;
	cmd.src1Shift=0;
	cmd.src2Shift =0;
//	cmd,0);	
	cmd.src1 = AWB_ADDR_RGain;
	cmd.dst =   AWB_ADDR_Tmp0;//AWB_ADDR_MOE_rect_left;	
	HAL_MoeConvertToQueue(&cmd, &(pt->rect_left)); // left	
	cmd.exchange = MOE_FUNC_Exchange;
	cmd.dst =   AWB_ADDR_Tmp2;//AWB_ADDR_MOE_rect_right;
	HAL_MoeConvertToQueue(&cmd, &(pt->rect_right)); // right	
	cmd.exchange = MOE_FUNC_NoExchange;
	cmd.src1 = AWB_ADDR_BGain;	
	cmd.dst =   AWB_ADDR_Tmp1;//AWB_ADDR_MOE_rect_down;
	HAL_MoeConvertToQueue(&cmd, &(pt->rect_down)); // down	
	cmd.exchange = MOE_FUNC_Exchange;
	cmd.dst =   AWB_ADDR_Tmp3;//AWB_ADDR_MOE_rect_up;
	HAL_MoeConvertToQueue(&cmd, &(pt->rect_up)); // up
	cmd.equation = MOE_EQUATION_Multiply;//MOE_EQUATION_Multiply;//and
	cmd.exchange = MOE_FUNC_NoExchange;
	cmd.func = MOE_FUNC_PointWithPoint;
	cmd.min = 0;
	//cmd.max = 0xffffffff;	
	cmd.src1Bit = 0;
	cmd.src2Bit= 0;
	cmd.dstBit=0;
	cmd.src1Shift=4;
	cmd.src2Shift=4;
	cmd.src1 = AWB_ADDR_Tmp0;//AWB_ADDR_MOE_rect_left;
	cmd.src2 = AWB_ADDR_Tmp2;//AWB_ADDR_MOE_rect_right;
	cmd.dst =  AWB_ADDR_Tmp4;//AWB_ADDR_MOE_rect_r;	
	HAL_MoeConvertToQueue(&cmd, &(pt->rect_r)); 	
	cmd.src1 = AWB_ADDR_Tmp1;//AWB_ADDR_MOE_rect_down;
	cmd.src2 = AWB_ADDR_Tmp3;//AWB_ADDR_MOE_rect_up;
	cmd.dst =  AWB_ADDR_Tmp5;//AWB_ADDR_MOE_rect_b;	
	HAL_MoeConvertToQueue(&cmd, &(pt->rect_b)); 	
	cmd.src1 = AWB_ADDR_Tmp4;//AWB_ADDR_MOE_rect_r;
	cmd.src2 = AWB_ADDR_Tmp5;//AWB_ADDR_MOE_rect_b;
	cmd.dst =  AWB_ADDR_Tmp6;//AWB_ADDR_MOE_rect;
//cmd.CmdQueueEnd=1;
	HAL_MoeConvertToQueue(&cmd, &(pt->rect)); //rect

	//cmd.src1Bit = 0;
	//cmd.dstBit = 0;
	cmd.max = 32;
	cmd.src1 = AWB_ADDR_Tmp6;//AWB_ADDR_MOE_rect;
	cmd.src2 = AWB_ADDR_Wp;//AWB_ADDR_MOE_wp;
	if(wp){
		cmd.dst = AWB_ADDR_Wp;//AWB_ADDR_MOE_wp;
	}else{
		cmd.dst = AWB_ADDR_Tmp7;//AWB_ADDR_MOE_rect_wp;
	}	
	HAL_MoeConvertToQueue(&cmd, &(pt->rect_wp)); 
	if(idx<AWB_RECT_NUM){
		UINT32 addr = AWB_ADDR_Out+ 12ul*(idx+6);
		
		cmd.equation = MOE_EQUATION_Summation;
		cmd.max = 0x7fffffff;
		cmd.sum=0;
		cmd.dst =  addr;//AWB_ADDR_MOE_rect_sum_wp;
	//cmd.CmdQueueEnd=1;
		HAL_MoeConvertToQueue(&cmd, &(pt0->rect_sum_wp)); 
		cmd.src2Bit = 1;
		if(wp){
			cmd.src1 = AWB_ADDR_Wp;//AWB_ADDR_MOE_wp;
		}else{
			cmd.src1 = AWB_ADDR_Tmp7;//AWB_ADDR_MOE_rect_wp;
		}
		cmd.src2 = AWB_ADDR_RGain;
		cmd.dst =   addr+4;//AWB_ADDR_MOE_rect_sum_r;
		HAL_MoeConvertToQueue(&cmd, &(pt0->rect_sum_r)); 
		cmd.src2 = AWB_ADDR_BGain;
		cmd.dst =   addr+8;//AWB_ADDR_MOE_rect_sum_b;
	//cmd.CmdQueueEnd=1;
		HAL_MoeConvertToQueue(&cmd, &(pt0->rect_sum_b)); 
	}
}
#endif
void awbMoeRectItem(awb_moe_rect_t* ptRect,UINT8 i,UINT16 *rect,UINT8 mode)
{
	if(!ptRect){return;}
	//i = AWB_RECT_Sward;
	awbMoeRect((awb_moe_rect_t*)ptRect,i,mode);
	i<<=2;
	moeCpWrite(ptRect->rect_left,	rect[i]);	
	moeCpWrite(ptRect->rect_right,	rect[i+1]);
	moeCpWrite(ptRect->rect_down,	rect[i+2]);
	moeCpWrite(ptRect->rect_up,	rect[i+3]);
}
void awbConvertCT2Gain(UINT16 ct,UINT16* rg,UINT16* bg,UINT16* gain,UINT16* thr)
{
	//2000K: thr[0],thr[8+1]
	//2500K: gain[0],gain[4]
	//4500K: gain[1],gain[5]
	//6500K: gain[2],gain[6]
	//7000K: thr[5],thr[8+4]
	UINT16 c0 = 2000, x0 = thr[0],y0=thr[8+1];
	UINT16 c1 = 4500, x1 = gain[1], y1=gain[1];
	UINT16 c2 = 7000, x2 = thr[5], y2 = thr[8+4];
//	UINT16 rg0,rg1,bg0,bg1;
	

	if(ct<4500){
		if(ct<2000){ct=2000;}
		*rg = x1-(UINT32)(c1-ct)*(x1-x0)/(c1-c0);
		*bg = y1+(UINT32)(c1-ct)*(y0-y1)/(c1-c0);	
	}
	else {
		if(ct>7000){ct=7000;}
		*rg = x1+(UINT32)(ct-c1)*(x2-x1)/(c2-c1);
		*bg = y1-(UINT32)(ct-c1)*(y1-y2)/(c2-c1);				
	}
	printf("ct:%u k(%x,%x)\n",ct,*rg,*bg);
}
/**
 * @fn        void awbMoeCmdItem(AWB_SCENE_ctt_t* ptFilter,awb_moe_raw_t* ptRaw, awb_moe_rect_t* ptRect,awb_moe_slop_t *ptSlopL,awb_moe_slop_t *ptSlopH,awb_moe_spot_t *ptSpot,awb_moe_rect_sum_t* ptRectSward, ,awb_moe_rect_sum_t* ptRectSky,awb_moe_rect_sum_t* ptRectFlash,awb_moe_buf_t* ad,UINT8 idx)
 * @brief     create all scene jobqueue and save to dram
 * @param     [in] ptFilter	:awb ctt params struct
 * @param     [out] ptRaw	:raw convert jobqueue pointer
 * @param     [out] ptRect	:rect filter jobqueue pointer
 * @param     [out] ptSlopL	:slop low filter jobqueue pointer
 * @param     [out] ptSlopH	:slop high filter jobqueue pointer
 * @param     [out] ptSpot	:spot filter jobqueue pointer
 * @param     [out] ptRectSward	:sward rect filter jobqueue pointer
 * @param     [out] ptRectSky	:sky rect filter jobqueue pointer
 * @param     [out] ptRectFlash	:flash rect filter jobqueue pointer
 * @param     [in] ad		:buf struct pointer
 * @param     [in] idx		:scene list index
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void awbMoeCmdItem(
	awb_ctt_t* ptCtt
	,awb_moe_raw_t* ptRaw
	,awb_moe_rect_t* ptRect
	,awb_moe_slop_t *ptSlopL
	,awb_moe_slop_t *ptSlopH
	,awb_moe_spot_t *ptSpot
	,awb_moe_rect_t* ptRectSward
	,awb_moe_rect_t* ptRectSky
	,awb_moe_rect_t* ptRectOffice
	,awb_moe_rect_t* ptRectFlash
	,awb_moe_rect_t* ptRectCalib
	//,awb_moe_rect_sum_t* ptRectOne
	,UINT8 idx
	,UINT8 bayer
	)
{
	UINT8 i;	
	UINT16 k0,k1;
	UINT16 thr[4*4];
	awb_ctt_t* ctt = (awb_ctt_t*)ptCtt;
	UINT16 *gain = ctt->ctc;
	UINT8 *ratio = ctt->ratio+16*idx;
	UINT16 *rect=ctt->rect;
	
	//if(!ptRectOne){
		for(i=0;i<16;i++)	{
			thr[i] = (UINT32)gain[i>>1]*ratio[i]>>7;
			//#if AWB_Debug_Print
			//printf("%bx-%2bx,%4x*%bx/128=%4x\n",idx,i,gain[i>>1],ratio[i],thr[i]);
			//#endif
		}
	//}
	
	if(ptRaw){
		awbMoeRaw(ptRaw,bayer);
	}
	if(ptRect){
		UINT16 rg0,rg1,bg0,bg1;
		awbConvertCT2Gain(awbctmin,&rg0,&bg1,gain,thr);
		awbConvertCT2Gain(awbctmax,&rg1,&bg0,gain,thr);		
		awbMoeRect(ptRect,0xff,0);		
		moeCpWrite(ptRect->rect_left,		rg0);//thr[0]);	
		moeCpWrite(ptRect->rect_right,	rg1);//thr[5]);
		moeCpWrite(ptRect->rect_down,	bg0);//thr[8+4]);
		moeCpWrite(ptRect->rect_up,		bg1);//thr[8+1]);

		/*awbMoeRect(ptRect,0xff,0);
		moeCpWrite(ptRect->rect_left,		thr[0]);	
		moeCpWrite(ptRect->rect_right,	thr[5]);
		moeCpWrite(ptRect->rect_down,	thr[8+4]);
		moeCpWrite(ptRect->rect_up,		thr[8+1]);*/
	}
	if(ptSlopL){
		UINT32 k;
		k = (UINT32)(thr[8+0]-thr[8+2])*256/(thr[2]-thr[0]);
		k0 = (k<0xffff)?k:0xffff;		
		k = (UINT32)(thr[8+2]-thr[8+4])*256/(thr[4]-thr[2]);
		k1 = (k<0xffff)?k:0xffff;
		//printf("##%x,%x\n",k0,k1);
		awbMoeSlop(ptSlopL,0);		
		moeCpWrite(ptSlopL->slop_dx,		thr[2]-1);
		moeCpWrite(ptSlopL->slop_dx_flag,		thr[2]-1);
		moeCpWrite(ptSlopL->slop_dy,			thr[8+2]-1);
		moeCpWrite(ptSlopL->slop_dy_flag,		thr[8+2]-1);
		moeCpWrite(ptSlopL->slop_cmp_k0,k0-1);		
		moeCpWrite(ptSlopL->slop_cmp_k1,k1+1);
	}
	if(ptSlopH){
		UINT32 k;
		k = (UINT32)(thr[8+1]-thr[8+3])*256/(thr[3]-thr[1]);
		k0 = (k<0xffff)?k:0xffff;
		k = (UINT32)(thr[8+3]-thr[8+5])*256/(thr[5]-thr[3]);
		k1 = (k<0xffff)?k:0xffff;
		//printf("##%x,%x\n",k0,k1);
		awbMoeSlop(ptSlopH,1);		
		moeCpWrite(ptSlopH->slop_dx,		thr[3]-1);
		moeCpWrite(ptSlopH->slop_dx_flag,	thr[3]-1);
		moeCpWrite(ptSlopH->slop_dy,		thr[8+3]-1);
		moeCpWrite(ptSlopH->slop_dy_flag,	thr[8+3]-1);		
		moeCpWrite(ptSlopH->slop_cmp_k0,k0-1);		
		moeCpWrite(ptSlopH->slop_cmp_k1,k1+1);
	}
	if(ptSpot){
		// d = y - (x+c0) 
		// c0 = y0-x0;
		/*
		UINT16 c0,dis4,dis14,dis15,dis5;	
		
		c0= gain[4+1]-gain[1];
		dis4= gain[4+0] - (gain[0] + c0);
		dis5= gain[2]  + c0 - gain[4+2];
		dis14 = dis4>>2;
		dis15 = dis5>>2;
		dis4 = dis14*3;
		dis5 = dis15*3;
		
		awbMoeSpot(ptSpot);
		moeCpWrite(ptSpot->dis_rgain_c0,c0);
		moeCpWrite(ptSpot->spot_0,dis4);
		moeCpWrite(ptSpot->spot_1,dis14);
		moeCpWrite(ptSpot->spot_2,-dis15);
		moeCpWrite(ptSpot->spot_3,-dis5);		
		moeCpWrite(ptSpot->spot_4,-dis5+1);
		*/
		/*
		SINT16 dis; 
		SINT16 dis4,dis14,dis15,dis5;	
		SINT16 dis0,dis1,dis2;
		dis0 = (SINT16)gain[4+0] - (SINT16)gain[0] ;
		dis1 = (SINT16)gain[4+1] - (SINT16)gain[1] ;
		dis2 = (SINT16)gain[4+2] - (SINT16)gain[2] ;

		dis = (dis0-dis1)/4;
		dis4 = dis0 - dis;
		dis14 = dis1+dis;

		dis = (dis1-dis2)/4;
		dis15 = dis1-dis;
		dis5 = dis2+dis;

		awbMoeSpot(ptSpot);
		//moeCpWrite(ptSpot->dis_rgain_c0,c0);
		moeCpWrite(ptSpot->spot_0,dis4);
		moeCpWrite(ptSpot->spot_1,dis14);
		moeCpWrite(ptSpot->spot_2,dis15);
		moeCpWrite(ptSpot->spot_3,dis5);		
		moeCpWrite(ptSpot->spot_4,dis5+1);
		*/
		SINT16 dis3; 
		SINT16 dis4;//,dis5;	
		SINT16 dis0,dis1,dis2;
		//dis0 = (SINT16)gain[4+0] - (SINT16)gain[0] ;
		//dis2 = (SINT16)gain[4+1] - (SINT16)gain[1] ;
		//dis1 = (dis0+dis2)/2;
		
		
		if(idx){
			dis4 = (SINT16)gain[4+2];			
			dis2 = (SINT16)gain[4+1];	
			dis0 = (SINT16)gain[4+0];
			dis1 = ((SINT16)gain[4+1] +gain[4+0])/2;
			dis3 = ((SINT16)gain[4+1] +gain[4+2])/2;
		}else{
			dis0 = (SINT16)gain[4+0] - (SINT16)gain[0] ;
			dis2 = (SINT16)gain[4+1] - (SINT16)gain[1] ;
			dis1 = (dis0+dis2)/2;
			dis4 = (SINT16)gain[4+2] - (SINT16)gain[2] ;
			dis3 = (dis2+dis4)/2;
		}
		//printf("dis:%d,%d,%d,%d,%d\n",dis0,dis1,dis2,dis3,dis4);
		awbMoeSpot(ptSpot,idx);
		//moeCpWrite(ptSpot->dis_rgain_c0,c0);
		moeCpWrite(ptSpot->spot_0,dis0);
		moeCpWrite(ptSpot->spot_1,dis1);
		moeCpWrite(ptSpot->spot_2,dis2);
		moeCpWrite(ptSpot->spot_3,dis3);		
		moeCpWrite(ptSpot->spot_4,dis4);
		moeCpWrite(ptSpot->spot_5,dis4+1);
	}
	awbMoeRectItem(ptRectSward,AWB_RECT_Sward,rect,1);
	awbMoeRectItem(ptRectSky,AWB_RECT_Sky,rect,2);
	awbMoeRectItem(ptRectOffice,AWB_RECT_Office,rect,2);
	awbMoeRectItem(ptRectFlash,AWB_RECT_Flash,rect,2);
	awbMoeRectItem(ptRectCalib,AWB_RECT_Calibration,rect,2);
	#if 0
	if(ptRectSward){		
		//i = ((idx == AWB_SCENE_Normal)?AWB_RECT_Calibration:AWB_RECT_Sward);
		i = AWB_RECT_Sward;
		awbMoeRect((awb_moe_rect_t*)ptRectSward,i,1);
		i<<=2;
		moeCpWrite(ptRectSward->rect_left,	rect[i]);	
		moeCpWrite(ptRectSward->rect_right,	rect[i+1]);
		moeCpWrite(ptRectSward->rect_down,	rect[i+2]);
		moeCpWrite(ptRectSward->rect_up,	rect[i+3]);
	}
	if(ptRectSky){
		//i = ((idx == AWB_SCENE_Normal)?AWB_RECT_Flash:AWB_RECT_Sky);
		i = AWB_RECT_Sky;
		awbMoeRect((awb_moe_rect_t*)ptRectSky,i,2);
		i<<=2;
		moeCpWrite(ptRectSky->rect_left,		rect[i]);	
		moeCpWrite(ptRectSky->rect_right,	rect[i+1]);
		moeCpWrite(ptRectSky->rect_down,	rect[i+2]);
		moeCpWrite(ptRectSky->rect_up,		rect[i+3]);
	}
	if(ptRectOffice){
		//i = ((idx == AWB_SCENE_Normal)?AWB_RECT_Flash:AWB_RECT_Sky);
		i = AWB_RECT_Office;
		awbMoeRect((awb_moe_rect_t*)ptRectOffice,i,2);
		i<<=2;
		moeCpWrite(ptRectOffice->rect_left,	rect[i]);	
		moeCpWrite(ptRectOffice->rect_right,	rect[i+1]);
		moeCpWrite(ptRectOffice->rect_down,	rect[i+2]);
		moeCpWrite(ptRectOffice->rect_up,	rect[i+3]);
	}
	if(ptRectFlash){
		//i = ((idx == AWB_SCENE_Normal)?AWB_RECT_Flash:AWB_RECT_Sky);
		i = AWB_RECT_Flash;
		awbMoeRect((awb_moe_rect_t*)ptRectFlash,i,2);
		i<<=2;
		moeCpWrite(ptRectFlash->rect_left,	rect[i]);	
		moeCpWrite(ptRectFlash->rect_right,	rect[i+1]);
		moeCpWrite(ptRectFlash->rect_down,	rect[i+2]);
		moeCpWrite(ptRectFlash->rect_up,		rect[i+3]);
	}
	if(ptRectCalib){
		//i = ((idx == AWB_SCENE_Normal)?AWB_RECT_Flash:AWB_RECT_Sky);
		i = AWB_RECT_Calibration;
		awbMoeRect((awb_moe_rect_t*)ptRectCalib,i,2);
		i<<=2;
		moeCpWrite(ptRectCalib->rect_left,	rect[i]);	
		moeCpWrite(ptRectCalib->rect_right,	rect[i+1]);
		moeCpWrite(ptRectCalib->rect_down,	rect[i+2]);
		moeCpWrite(ptRectCalib->rect_up,		rect[i+3]);
	}
	/*
	if(ptRectOne){
		i = idx<<2;		
		awbMoeRect((awb_moe_rect_t*)ptRectOne,AWB_RECT_One,0);
		moeCpWrite(ptRectOne->rect_left,	rect[i]);	
		moeCpWrite(ptRectOne->rect_right,	rect[i+1]);
		moeCpWrite(ptRectOne->rect_down,	rect[i+2]);
		moeCpWrite(ptRectOne->rect_up,		rect[i+3]);
	}
	*/
	#endif
}

/**
 * @fn        void awbMoeCmdInit(UINT8 *ptCmd,AWB_SCENE_ctt_t* ptFilter ,awb_moe_buf_t* ad,UINT8 idx)
 * @brief     create jobqueue of scene
 * @param     [out] ptCmd	:job queue pointer
 * @param     [in] ptFilter	:awb ctt param
 * @param     [in] ad		:buf struct point
 * @param     [in] idx		:scene idx
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void awbMoeCmdInit(
		UINT8 *ptCmd
		,awb_ctt_t* ptCtt
		,UINT8 idx
		)
{
	UINT16 size;
	UINT16 cnt=0;
	UINT8 bayerpv = awb.rawFmt & 0x0f;
	UINT8 bayersn = awb.rawFmt >> 4;
	
	//if(idx == AWB_SCENE_Normal){
		awb_moe_normal_t* pt = ptCmd;		
		size = sizeof(awb_moe_normal_t);		
		awbMoeCmdItem(
				ptCtt, 
				&(pt->raw),
				&(pt->rect), 
				&(pt->slopL), 
				&(pt->slopH),
				&(pt->spot), 
				(awb_moe_rect_t*)&(pt->rectSward), 
				(awb_moe_rect_t*)&(pt->rectSky), 
				(awb_moe_rect_t*)&(pt->rectOffice), 
				(awb_moe_rect_t*)&(pt->rectFlash), 
				(awb_moe_rect_t*)&(pt->rectCalib), 
				idx,
				bayerpv
				);
	/*}else if(idx == AWB_SCENE_Outdoor){
		awb_moe_outdoor_t*pt = ptCmd;	
		size = sizeof(awb_moe_outdoor_t);
		
		awbMoeCmdItem(
				ptCtt, 
				&(pt->raw),
				&(pt->rect), 
				&(pt->slopL), 
				&(pt->slopH),
				&(pt->spot), 
				(awb_moe_rect_t*)&(pt->rectSward), 
				(awb_moe_rect_t*)&(pt->rectSky), 
				(awb_moe_rect_t*)&(pt->rectOffice), 
				NULL,
				idx,
				bayerpv		
				);	
	}*/
	//rect one
	/*
	else if(idx == AWB_SCENE_Flash)
	{//flash
		awb_moe_rectone_t*pt = ptCmd;
		size = sizeof(awb_moe_rectone_t);
		
		awbMoeCmdItem(
				ptCtt, 
				&(pt->raw),
				NULL, 
				NULL, 
				NULL,
				NULL, 
				NULL, 
				NULL, 
				&(pt->rectsum0), 
				AWB_RECT_Flash,
				bayersn
				);
	}
	else if(idx == AWB_SCENE_Calibration)
	{//Calibration
		awb_moe_rectone_t*pt = ptCmd;
		size = sizeof(awb_moe_rectone_t);
		
		awbMoeCmdItem(
				ptCtt, 
				&(pt->raw),
				NULL, 
				NULL, 
				NULL,
				NULL, 
				NULL, 
				NULL, 
				&(pt->rectsum0), 
				AWB_RECT_Calibration,
				bayerpv	
				);
	}
	else
	{
		return;
	}*/
	cnt = size/sizeof(moe_queue_t);
	HAL_MoeSetQueueEnd((moe_queue_t*)ptCmd,cnt-1);
	//HAL_DramWriteBytesAddr(pawbaddr->queue[idx], ptCmd, size, 0);
#if AWB_Debug_0	
	printf("--idx:%bx,size:%d,cnt:%d,addr:%lx\n",idx,size,cnt,pawbaddr->queue[idx]);
//#if AWB_Debug_0	

{// save queue to sd
	UINT8 fn[] = "Q0000000.DAT";
	fn[7] = idx+'0';
	dramFileSave(fn, pawbaddr->queue[idx], size);

	printf("--idx:%bx,size:%d,cnt:%d\n",idx,size,cnt);
}	
#endif
}

#if 0
/**
 * @fn        void awbMoeBufInit(awb_moe_buf_t *pt)
 * @brief     initialize buf struct
 * @param     [out] pt
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void awbMoeBufInit(awb_moe_buf_t *pt)
{
	UINT16 i;
	
	pt->queue[AWB_SCENE_Normal] = AWB_ADDR_Queue_normal;
	pt->queue[AWB_SCENE_Outdoor] =AWB_ADDR_Queue_outdoor;
	pt->queue[AWB_SCENE_Flash] = AWB_ADDR_Queue_flash;
	pt->queue[AWB_SCENE_Calibration] = AWB_ADDR_Queue_calib;

	pt->out = AWB_ADDR_Out;
	pt->wp = AWB_ADDR_Wp;
	
	pt->samp = AWB_ADDR_Samp;
	pt->rgain = AWB_ADDR_Gain;
	pt->bgain = AWB_ADDR_Gain+1024;
	
	pt->tmp[0] = AWB_ADDR_Tmp;
	//pt32 = pt->tmp;
	for(i=1;i<10;i++){
		pt->tmp[i] = pt->tmp[i-1] + AWB_SAMP_WCNT;
	}

	pt->rect_left 		= pt->tmp[0];
	pt->rect_right 	= pt->tmp[2];
	pt->rect_down 	= pt->tmp[1];
	pt->rect_up 		= pt->tmp[3];
	pt->rect_r 		= pt->tmp[4];
	pt->rect_b 		= pt->tmp[5];
	pt->rect 			= pt->tmp[6];
	pt->rect_wp 		= pt->tmp[7];	

	//slop_low
	pt->slop_dx = pt->tmp[0];
	pt->slop_dy = pt->tmp[2];
	pt->slop_k = pt->tmp[4];//8.8
	pt->slop_cmp_k0 = pt->tmp[0];
	pt->slop_cmp_k1 = pt->tmp[2];
	pt->slop_dx_flag = pt->tmp[1];
	pt->slop_dy_flag = pt->tmp[3];
	pt->slop_k0dx = pt->tmp[4];
	pt->slop_k1dy = pt->tmp[5];
	pt->slop_left_down = pt->tmp[0];;
	pt->slop_and  = pt->tmp[1];
	
	pt->slop_low  = pt->tmp[6];

	pt->slop_high  = pt->tmp[7];
	//slop
	pt->slop  = pt->tmp[0];
	//spot
	//pt->spot  = pt->tmp[1];
	//pt->spot_wp  = pt->tmp[2];	
	
	//dis
	pt->dis_rgain_c0 = pt->tmp[0];
	pt->dis_sign = pt->tmp[6];
	//spot	
	//pt->spot_40 = pt->tmp[4];
	//pt->spot_10 = pt->tmp[1];
	//pt->spot_50 = pt->tmp[5];
	//pt->spot_20 = pt->tmp[2];
	
	pt->spot_0 = pt->tmp[0];
	pt->spot_1 = pt->tmp[1];
	pt->spot_2 = pt->tmp[2];
	pt->spot_3 = pt->tmp[3];	
	pt->spot_4 = pt->tmp[4];

	//spot sum
	pt->spot_rgain_wp = pt->rgain;
	pt->spot_bgain_wp = pt->bgain;
	pt->spotSum = pt->out;

	//halDramFill(pt->tmp[0], 1024*10, 0);
	//halDramFill(ptAddr->addrQueue, 1024*16, 0);

	
	//printf("normal:%lx,outdoor:%lx,flash:%lx,%lx,tmp:%lx\n",ptAddr->addrQueue[AWB_SCENE_Normal],ptAddr->addrQueue[AWB_SCENE_Outdoor],ptAddr->addrQueue[AWB_SCENE_Flash],(UINT32)sizeof(awb_moe_normal_t),ptAddr->addrTmp);
{
	printf("out:%lx,samp:%lx,tmp:%lx,gain:%lx,wp:%lx\n"
		,pt->out
		,pt->samp
		,pt->tmp[0]
		,pt->rgain
		,pt->wp
	);
	printf("queue(%lx,%lx,%lx),size(%lx,%lx,%lx),cnt(%lx,%lx,%lx)\n"
		,pt->queue[AWB_SCENE_Normal],pt->queue[AWB_SCENE_Outdoor],pt->queue[AWB_SCENE_Flash]
		,(UINT32)sizeof(awb_moe_normal_t),(UINT32)sizeof(awb_moe_outdoor_t),(UINT32)sizeof(awb_moe_rectone_t)
		,(UINT32)sizeof(awb_moe_normal_t)/sizeof(moe_queue_t)
		,(UINT32)sizeof(awb_moe_outdoor_t)/sizeof(moe_queue_t)
		,(UINT32)sizeof(awb_moe_rectone_t)/sizeof(moe_queue_t)
		);
}
}
#endif
/**
 * @fn        void awbMoeQueueCreate(awb_addr_t *ptAddr, AWB_SCENE_ctt_t *ptFilter)
 * @brief     initialize awb moe
 * @param     [in] ptAddr	:awb dram addr struct
 * @param     [in] ptFilter	:awb ctt param struct
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void awbMoeQueueCreate(awb_ctt_t* ptCtt)
{
	UINT8 sts;
	awb_moe_queue_t xdata* ptQueue;
	UINT32 que = pawbaddr->que;
	
	ENTER_CRITICAL(sts);
	HAL_CpuRamMapSet(que, 16UL*1024UL, (void xdata* xdata*)&ptQueue);
	
	awbMoeCmdInit((UINT8*)&(ptQueue->normal),ptCtt,AWB_SCENE_Normal);
	
	awbMoeCmdInit((UINT8*)&(ptQueue->outdoor),ptCtt,AWB_SCENE_Outdoor);

	//awbMoeCmdInit((UINT8*)&(ptQueue->flash),ptCtt,AWB_SCENE_Flash);

	//awbMoeCmdInit((UINT8*)&(ptQueue->calibration),ptCtt,AWB_SCENE_Calibration,pvBayer);	
	
	HAL_CpuRamMapRcv();
	EXIT_CRITICAL(sts);	
	//HAL_GprmDramDmaAdv(que<<1, pawbaddr->queue[0]<<1, 1024*7, 0);
}

void awbMoeModifyYThr(UINT8 ymin,UINT8 ymax,UINT8 wpmax)
{
	UINT32 addr = pawbaddr->queue[0];
	awb_moe_raw_t* raw = 0;
	UINT32 offset = (UINT32)&(raw->wp.max0);
	UINT8 t0=0,t1=0;
	XBYTE[REG_Dram_AddressErr] &= ~0x01;
	//HAL_DramSetAddr(addr+offset/2, 1);
	//HAL_DramWordDataRead(&t0, &t1);
	//printf("wp%lx,%bu,%bu,%bu\n",offset,t0,t1,wpmax);
	HAL_DramSetAddr(addr+offset/2, 0);
	HAL_DramWordDataWrite(wpmax, t1);
	

	offset = (UINT32)&(raw->thr.low.src20);
	//HAL_DramSetAddr(addr+offset/2, 1);
	//HAL_DramWordDataRead(&t0, &t1);
	//printf("y%lx,%bu,%bu,%bu\n",offset,t0,t1,ymin);
	HAL_DramSetAddr(addr+offset/2, 0);
	HAL_DramWordDataWrite(ymin, t1);
	

	offset = (UINT32)&(raw->thr.high.src20);
	//HAL_DramSetAddr(addr+offset/2, 1);
	//HAL_DramWordDataRead(&t0, &t1);
	//printf("y%lx,%bu,%bu,%bu\n",offset,t0,t1,ymax);
	HAL_DramSetAddr(addr+offset/2, 0);
	HAL_DramWordDataWrite(ymax, t1);
	XBYTE[REG_Dram_AddressErr] |= 0x01;
}
#endif
