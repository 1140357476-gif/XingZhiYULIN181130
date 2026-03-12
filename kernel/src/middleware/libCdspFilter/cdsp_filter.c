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
 * @file      Cdsp_filter.c
 * @brief     
 * @author    Will Chiu
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "cdsp_filter.h"
#include "cdsp_debug.h"
#include "cdsp_task.h"
#include "sp1k_rsvblk_api.h"
#include "initio.h"
#include "Asicreg.h"

#include "reg_def.h"
#include "hal_cdsp_iq.h"
#include "front_logi.h"
#include "hal_front.h"
#include "cmd_test.h"
#include "math.h"
#include "sp1k_cdsp_api.h"
#include "pv_task.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define __FILE	__FILE_ID_CDSP_FILTER__

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT8 G_PvDyTDEn;
UINT8 G_PvDyTDLevel;
UINT8 cdspPvDyTDSwitchFlowStatus;

UINT8 G_PvDyTDSwitchClkEnTmp;
UINT8 G_PvDyTDSwitchClkEn;
UINT8 G_PvDyTDRegSet;
UINT8 G_PvDyTDGateSet;
UINT8 G_PvDyTDPclkSet;


 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT8 frontPvDyTDSwitchClkStatus;

/**************************************************************************
 *               I N T E R N A L    F U N C T I O N D E C L A R A T I O N S*
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/


/**
 * @fn        void cdspEdgeFilterSet(UINT8 level, UINT8 opt)
 * @brief     Several sets of edge fileter parameters. It needs resource, "EdgeCoring.bin".
 * @param     [in] level : 0~10 for edge ( higher -> stronger)
 * @param     [in] opt :  0: capture 
 *\n					  1: preview
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2011-3-18
 * @todo      N/A
 * @bug       N/A
*/
void
cdspEdgeFilterSet(
	UINT8 level,
	UINT8 opt
)
{
#if (CDSP_IQ_TEST==0)
	level=level;
	opt=opt;
#else
	UINT8 i,lsb,msb;
	

	#if 1
		UINT8 edge_coef[EDGE_REG_NUM][EDGE_LEVEL];
	#else
		UINT8 edge_coef[EDGE_REG_NUM][EDGE_LEVEL] = {
			{ 0x28,	0x28, 0x28,	0x28, 0x28,	0x28, 0x28,	0x28, 0x28,	0x28, 0x28}	,	/*0x216C*/
			{ 0xff, 0xff, 0xff,	0xff, 0xff,	0xff, 0xff,	0xff, 0xff,	0xff, 0xff}	,	/*0x21D2*/
			{ 0x02, 0x02, 0x02,	0x02, 0x02,	0x02, 0x02,	0x02, 0x02,	0x02, 0x02}	,	/*0x21D3*/
			{ 0x00, 0x00, 0x00,	0x00, 0x00,	0x00, 0x00,	0x00, 0x00,	0x00, 0x00}	,	/*0x21D4*/
			{ 0x18, 0x1e, 0x23,	0x29, 0x2e,	0x34, 0x3a,	0x3f, 0x45,	0x4a, 0x50}	,	/*0x21D5*/
			{ 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10} ,	/*0x21D6*/	
			{ 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38} ,	/*0x21D7*/
			{ 0x63, 0x63, 0x63,	0x63, 0x63,	0x63, 0x63,	0x63, 0x63,	0x63, 0x63}	,	/*0x21D8*/					
			{ 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04}	,	/*0x21D9*/
			{ 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02}	,	/*0x21DE*/
			{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}	,	/*0x21DD*/
			{ 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09}	,	/*0x21DC*/
			{ 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09}	,	/*0x21DB*/
			{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}	,	/*0x21DA*/
		};
	#endif


	UINT16 edge_reg[EDGE_REG_NUM] = {
		 REG_Cdsp_intpllowthr,REG_Cdsp_EdgeEn,REG_Cdsp_Yhtclamp,REG_Cdsp_Yhtclamp,
		 REG_Cdsp_ygain0,REG_Cdsp_ygain1,REG_Cdsp_yhtlowthr,REG_Cdsp_yhthighthr,
		 REG_Cdsp_yhtbw,REG_Cdsp_ftr0,REG_Cdsp_ftr1,REG_Cdsp_ftr2,REG_Cdsp_ftr3,
		 REG_Cdsp_ftr4,REG_Cdsp_ftr5,REG_Cdsp_Medmode
	};

	sp1kNandRsvRead(0x3d, (K_SDRAM_CommonEndAddr<< 1));


	if (opt==0){ /*still*/
		M_DRAM_SetStartAddress((UINT32)(K_SDRAM_CommonEndAddr+4+EDGE_REG_NUM*EDGE_LEVEL/2), 1);
	}
	else{
		M_DRAM_SetStartAddress((UINT32)(K_SDRAM_CommonEndAddr+4), 1);
	}
	
	for (i=0;i<EDGE_REG_NUM*EDGE_LEVEL;i=i+2) {
		M_DRAM_ReadWord(lsb, msb);
		edge_coef[i%16][i/16]=lsb;
		edge_coef[i%16+1][i/16]=msb;		
		//cdspPrintf("data=0x%bx,0x%bx\n",lsb,msb);
	}	

	if (level>(EDGE_LEVEL-1)){
		level=EDGE_LEVEL;	
	}
	
	for(i = 0; i < EDGE_REG_NUM ; i++) {
		XBYTE[edge_reg[i]] = edge_coef[i][level];

		cdspPrintf("%bd reg addr=0x%x,val=0x%bx\n",i,edge_reg[i],edge_coef[i][level]);
	}	

	if (opt==0){ /*still*/
		
		cdspStillEdgeParaSet(edge_coef[2][level],
							edge_coef[3][level],
							edge_coef[4][level],
							edge_coef[5][level],
							edge_coef[6][level],
							edge_coef[7][level],
							edge_coef[8][level]);

		cdspStillEdgeFilterSet(edge_coef[9][level],
								edge_coef[10][level],
								edge_coef[11][level],
								edge_coef[12][level],
								edge_coef[13][level],
								edge_coef[14][level]);		
	}	
	else{
		cdspPvEdgeFilterEnSet(edge_coef[1][level]&0x0f);
							
		cdspPvEdgeParaSet(edge_coef[1][level],
							edge_coef[2][level],
							edge_coef[3][level],
							edge_coef[4][level],
							edge_coef[5][level],
							edge_coef[6][level],
							edge_coef[7][level]);

		cdspPvEdgeFilterSet(edge_coef[8][level],
							edge_coef[9][level],
							edge_coef[10][level],
							edge_coef[11][level],
							edge_coef[12][level],
							edge_coef[13][level]);
	}
#endif	
}

/**
 * @fn        void cdspCoringFilterSet(UINT8 level)
 * @brief     6 sets of coring parameter. It only works for capture mode.
 * @param     [in] level  :  0~5  (higher -> stronger)
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2011-3-18
 * @todo      N/A
 * @bug       N/A
*/
void 
cdspCoringFilterSet(
	UINT8 level
)
{
#if (CDSP_IQ_TEST==0)
	level=level;
#else
	UINT16 i;
	UINT8 lsb,msb;
	
	#if 1
		UINT8 coring_coef[CORING_REG_NUM][CORING_LEVEL];	
	#else
		UINT8 coring_coef[CORING_REG_NUM][CORING_LEVEL]={
			{	2	,	3	,	4	,	5	,	6	,	7	}	,	//0x22A0
	 		{	2   ,  	3	,	4	,	5	,	5	,	6	}	,	//0x22A1
	 		{	2   ,  	3  	,	4	,	5	,	5	,	6	}	,	//0x22A2
			{	1   ,  	2  	,	3	,	4	,	4	,	5	}	,	//0x22A3
			{	1   ,  	2	,	3	,	4	,	4	,	5	}	,	//0x22A4
			{	1   ,  	2	,	2	,	3	,	4	,	5	}	,	//0x22A5
			{	1   ,  	2  	,	2	,	3	,	4	,	5	}	,	//0x22A6
			{	1   ,  	2  	,	2	,	3	,	3	,	4	}	,	//0x22A7
			{	1   ,  	2	,	2	,	3	,	3	,	4	}	,	//0x22A8
			{	1   ,  	1	,	2	,	2	,	3	,	3	}	,	//0x22A9
			{	1   ,  	1  	,	2	,	2	,	3	,	3	}	,	//0x22AA
			{	1   ,  	1	,	2	,	2	,	2	,	2	}	,	//0x22AB
	 		{	1   ,  	1  	,	2	,	2	,	2	,	2	}	,	//0x22AC
	 		{	1   ,  	1	,	1	,	1	,	2	,	2	}	,	//0x22AD
			{	1   ,  	1	,	1	,	1	,	2	,	2	}	,	//0x22AE
			{	1   ,  	1	,	1	,	1	,	1	,	1	}		//0x22AF
		};

	#endif

	sp1kNandRsvRead(0x3d, (K_SDRAM_CommonEndAddr<< 1));
	
	M_DRAM_SetStartAddress((UINT32)(K_SDRAM_CommonEndAddr+(EDGE_REG_NUM*EDGE_LEVEL+4)), 1);
	
	for (i=0;i<(CORING_REG_NUM*CORING_LEVEL);i=i+2) {
		M_DRAM_ReadWord(lsb, msb);
		
		coring_coef[i%17][i/17]=lsb;
		coring_coef[((i+1)%17)][(i+1)/17]=msb;		
		//cdspPrintf("data=0x%bx,0x%bx\n",lsb,msb);
	}	
	
	if (level>(CORING_LEVEL-1)){
		level=CORING_LEVEL-1;	
	}

	for(i = 0; i < CORING_REG_NUM ; i++) {
		XBYTE[0x22a0+i] = coring_coef[i][level];
		//XBYTE[0x22a0+i] = ;
		cdspPrintf("reg addr=0x%x,val=0x%bx\n",0x22a0+i,coring_coef[i][level]);
	}
#endif	
}	

/**
 * @fn        UINT8 cdspTempDenoiseEn(UINT8 en, UINT8 level)
 * @brief     enable/disable normal temporal denoise and set level of de-noise
 * @param     [in] en    1: on   0: off
 * @param     [in] level	0: weak  1: normal  2:strong
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2011-3-23
 * @todo      N/A
 * @bug       N/A
*/
void 
cdspTempDenoiseEn(
   UINT8 en,
   UINT8 level
)
{

	UINT16 hSize,vSize;	
	UINT8 wid;

	//cdspPrintf("en=%bx,level=%bx\n",en,level);
	pvSizeGet(&hSize,&vSize);
	//cdspPrintf("h=%d,v=%d\n",hSize,vSize);
	HAL_CdspDarkTDSizeSet(hSize); 

	HAL_CdspTDCfg(0,0,1);

	if (en==0){
		HAL_CdspTDEnSet(0);
		HAL_FrontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
		XBYTE[REG_Cdsp_disgatedv1xclk_high] &= ~0x02;
		cdspPrintf("td off\n");
	}
	else{

		switch (level){
			case 0:
				wid=3;
			break;
			case 1:
				wid=4;
			break;
			case 2:
				wid=5;
			break;
			default:
			break;
		}

		#if 1
		cdspPvDynamicTempDenoiseParaSet(0x18, 0xff, 0x00, wid, 0x18, 0xff, 0x00, wid);
		#else
		XBYTE[ REG_Cdsp_3Dtdlowyval ]	= 0x18;
		XBYTE[ REG_Cdsp_3Dtdhighyval ]	= 0xff;
		XBYTE[ REG_Cdsp_3Dtdlowythr ]	= 0x00;  /*Minimum Weighting*/	   
		XBYTE[ REG_Cdsp_3Dtdywid ]		= wid;  /*Y width*/	 
		XBYTE[ REG_Cdsp_3Dtdlowuvval ]	= 0x18;    
		XBYTE[ REG_Cdsp_3Dtdhighuvval ] = 0xff; 	
		XBYTE[ REG_Cdsp_3Dtdlowuvthr ]	= 0x00;  /*Minimum Weighting*/	
		XBYTE[ REG_Cdsp_3Dtduvwid ] 	= wid;  /*UV width*/		
		#endif	

	
		HAL_CdspTDEnSet(1);
		XBYTE[REG_Cdsp_disgatedv1xclk_high] |= 0x02;
		cdspPrintf("td on\n");
	}	
	//cdspPrintf("Preivew TD Test \n");   	
}

/**
 * @fn        UINT8 cdspPvDynamicTempDenoiseEn(UINT8 en, UINT8 level)
 * @brief     enable/disable preview dynamic ctrl temporal denoise (based on sensor gain) and set level of de-noise
 * @param     [in] en    1: on (after ae enable)  0: off (before ae disable)
 * @param     [in] level	0: weak  1: normal  2:strong
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2011-3-23
 * @todo      N/A
 * @bug       N/A
*/
void 
cdspPvDynamicTempDenoiseEn(
   UINT8 en,
   UINT8 level
)
{
	UINT8 wid;
	UINT16 hSize,vSize;	
	
	G_PvDyTDLevel = level;

	if (en==0){
		#if 1
		XBYTE[ REG_Cdsp_3Dtden ] |= 0x08;
		cdspPrintf("td off\n");
		#else
		XBYTE[ REG_Cdsp_3Dtden ] |= 0x08;
		//HAL_FrontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1); //rf--
		HAL_CdspTDEnSet(0); 	//td 'enable'
		HAL_CdspTDCfg(0,1,0);	//reset '1', svd'next vd', tdzupdateen'1'

		XBYTE[REG_Cdsp_disgatedv1xclk_high] &= ~0x02;
		cdspPrintf("td off\n");
		#endif
	}
	else{

		HAL_CdspTDCfg(1,1,0);	//reset '1', svd'next vd', tdzupdateen'1'

		//HAL_FrontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);

		HAL_CdspTDEnSet(1);		//td 'enable'

		pvSizeGet(&hSize,&vSize);
		HAL_CdspDarkTDSizeSet(hSize);

		switch (level){
			case 0:
				wid=3;
			break;
			case 1:
				wid=4;
			break;
			case 2:
				wid=5;
			break;
			default:
			break;
		}

		#if 1
		cdspPvDynamicTempDenoiseParaSet(0x18, 0xff, 0x00, wid, 0x18, 0xff, 0x00, wid);
		#else
		XBYTE[ REG_Cdsp_3Dtdlowyval ]	= 0x18;
		XBYTE[ REG_Cdsp_3Dtdhighyval ]	= 0xff;
		XBYTE[ REG_Cdsp_3Dtdlowythr ]	= 0x00;  /*Minimum Weighting*/	   
		XBYTE[ REG_Cdsp_3Dtdywid ]		= wid;  /*Y width*/	 
		XBYTE[ REG_Cdsp_3Dtdlowuvval ]	= 0x18;    
		XBYTE[ REG_Cdsp_3Dtdhighuvval ] = 0xff; 	
		XBYTE[ REG_Cdsp_3Dtdlowuvthr ]	= 0x00;  /*Minimum Weighting*/	
		XBYTE[ REG_Cdsp_3Dtduvwid ] 	= wid;  /*UV width*/		
		#endif
	
		//HAL_CdspTDEnSet(1);
		XBYTE[REG_Cdsp_disgatedv1xclk_high] |= 0x02;
		cdspPrintf("td on\n");
	}	

	G_PvDyTDEn = en;
	//cdspPrintf("Preivew TD Test \n");   	
}

/**
 * @fn        UINT8 cdspPvDynamicTempDenoiseParaSet(UINT8 tdlowyval,UINT8 tdhighyval,UINT8 tdlowythr,UINT8 tdywid,UINT8 tdlowuvval,UINT8 tdhighuvval,UINT8 tdlowuvthr,UINT8 tduvwid)
 * @brief     preview dynamic ctrl temporal denoise weight parameter set
 * @param     [in] none
 * @retval    none
 * @see       NULL
 * @author   LinJieCheng
 * @since     2011-3-23
 * @todo      N/A
 * @bug       N/A
*/
void 
cdspPvDynamicTempDenoiseParaSet(
	UINT8 tdlowyval,
	UINT8 tdhighyval,
	UINT8 tdlowythr,
	UINT8 tdywid,
	UINT8 tdlowuvval,
	UINT8 tdhighuvval,
	UINT8 tdlowuvthr,
	UINT8 tduvwid
)
{
	XBYTE[ REG_Cdsp_3Dtdlowyval ]	= tdlowyval;
	XBYTE[ REG_Cdsp_3Dtdhighyval ]	= tdhighyval;
	XBYTE[ REG_Cdsp_3Dtdlowythr ]	= tdlowythr;  /*Minimum Weighting*/	   
	XBYTE[ REG_Cdsp_3Dtdywid ]		= tdywid;  /*Y width*/	 
	XBYTE[ REG_Cdsp_3Dtdlowuvval ]	= tdlowuvval;    
	XBYTE[ REG_Cdsp_3Dtdhighuvval ] = tdhighuvval; 	
	XBYTE[ REG_Cdsp_3Dtdlowuvthr ]	= tdlowuvthr;  /*Minimum Weighting*/	
	XBYTE[ REG_Cdsp_3Dtduvwid ] 	= tduvwid;  /*UV width*/	
}

//#include "ae_debug.h"
//#include "gpio_api.h"

void
INTR_PvDynamicTempoalDenoiseCheck(
	void
)
{
	if (G_PvDyTDSwitchClkEnTmp != G_PvDyTDSwitchClkEn) {
		//aeStatusGpioByteOutSet(GPIO_BYTE_GEN1, 0x20, 0x20);	/* status 1 */
		cdspPvDyTDSwitchFlowStatus = 1;
		
		#if 0
		if (G_PvDyTDSwitchClkEn == FRONT_PV_DYNAMIC_TD_ENABLE) {
			printf("flow e\n");
		}
		else {
			printf("flow d\n");		
		}
		#endif
		
		frontIntrPreviewDynamicTempoalDenoiseSet(frontDevActGet(),G_PvDyTDSwitchClkEn);

		G_PvDyTDPclkSet = 1;
		G_PvDyTDSwitchClkEnTmp = G_PvDyTDSwitchClkEn;

		//aeStatusGpioByteOutSet(GPIO_BYTE_GEN1, 0x20, 0x00);	/* status 1 */
	}
	
}

void
INTR_PvDynamicTempoalDenoisePclkSet(
	void
)
{	
	if (G_PvDyTDPclkSet == 1) {
		//printf("pclk set\n");	
		
		G_PvDyTDPclkSet = 0;		

		if (G_PvDyTDSwitchClkEnTmp == FRONT_PV_DYNAMIC_TD_DISABLE) {
			
			//printf("reg [dis]\n");
			
			#if 1
			XBYTE[ REG_Cdsp_3Dtden ] |= 0x08;
			#endif
			
			//XBYTE[REG_Cdsp_disgatedv1xclk_high] &= ~0x02;
		}
		
		//aeStatusGpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x10);	/* status 2 */
		
		frontIntrPreviewDynamicTempoalDenoiseSet(frontDevActGet(),G_PvDyTDSwitchClkEn);	
		
		G_PvDyTDRegSet = 1;

		//aeStatusGpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x00);	/* status 2 */
	}
}

void
INTR_PvDynamicTempoalDenoiseRegSet(
	void
)
{
	if (G_PvDyTDRegSet == 1) {

		//printf("reg set\n");	
		
		G_PvDyTDRegSet = 0;
		
		if (G_PvDyTDSwitchClkEnTmp == FRONT_PV_DYNAMIC_TD_ENABLE) {
			
			//printf("reg [en]\n");
			
			#if 1
			XBYTE[ REG_Cdsp_3Dtden ] &= (~0x08);
			#endif
			
			//XBYTE[REG_Cdsp_disgatedv1xclk_high] |= 0x02;
		}

		G_PvDyTDGateSet = 0;//disable gating frame function

		if (G_PvDyTDGateSet == 0) {
			cdspPvDyTDSwitchFlowStatus = 0;
		}
	}
	
}

void
INTR_PvDynamicTempoalDenoiseGateSet(
	void
)
{
	
	
	if (G_PvDyTDGateSet == 1) {

		//printf("gate set\n");
	
		G_PvDyTDGateSet = 0;

		XBYTE[REG_Front_F2CdspGate] &= (~0x01);
	
		cdspPvDyTDSwitchFlowStatus = 0;
	}
	
}



/**
 * @fn        void logFun(float val)
 * @brief     log function (in order to link library code in bank15)
 * @param     [in] val
 * @retval    [out] log(val)
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-12-21
 * @todo      N/A
 * @bug       N/A
*/
float
logFun(
	float val
)
{
	return log(val);
}
