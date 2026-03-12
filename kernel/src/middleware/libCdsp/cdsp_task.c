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
 * @file      Cdsp_task.c
 * @brief     
 * @author    Will Chiu
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "hal_front.h"
#include "hal_front_para.h"
#include "hal_gpio.h"
#include "sp1k_cdsp_api.h"
#include "front_logi.h"
#include "ae_api.h"
#include "sp1k_ae_api.h"
#include "awb_api.h"//wangjg++
#include "sp1k_snap_api.h"
#include "sp1k_aud_api.h"
#include "cdsp_init.h"
#include "initio.h"
#include "sp1k_bp_api.h"
#include "util_api.h"
#include "hal_cdsp.h"
#include "hal_cdsp_core.h"
#include "hal_cdsp_iq.h"
#include "hal.h"
#include "dbg_def.h"
#include "doslink.h"
#include "hal_dram.h"
#include "snap_task.h"
#include "cdsp_task.h" 
#include "snap_core.h"
#include "hal_global.h"
#include "cdsp_debug.h"
#include "hal_moe.h"
#include "icat_stub.h"
#include "reg_def.h"
#include "os_api.h"
//#include "pv_task.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define __FILE	__FILE_ID_CDSP_TASK__


/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define CDSP_PARAM_SIZE ( 16 )
	
#define CDSP_POST_WB_H_SIZE 32
#define CDSP_POST_WB_V_SIZE 32
#define CDSP_POST_WB_EACH_V_SIZE 2
	
	
#ifdef CDSPTASK_REMOVE_PRINTF
#define printf(a)
#endif

#define CDSP_TASK_SUCCESS 0
#define CDSP_TASK_FAIL    1

#define CDSP_TASK_TAG_TODO
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT16 WB_Groffset;
UINT16 WB_Gboffset;
UINT16 WB_Roffset;
UINT16 WB_Boffset;
UINT8  WB_offseten;
	
UINT16 WB_Grgain;
UINT16 WB_Gbgain;
UINT16 WB_Rgain;
UINT16 WB_Bgain;
	
//UINT8 cdspParam1[ CDSP_PARAM_SIZE ];
//UINT8 cdspParam2[ CDSP_PARAM_SIZE ];

//static UINT8 cdspStillYEdge;
//static UINT8 cdspStillLEdge;
//static UINT8 cdspStillYCoring;
//static UINT8 cdspStillUvMedian;
static UINT8 cdspStillFilterEn;		/* 0x21d0 */	

static UINT8 cdspStillEdgeClamp;    /* 0x21d2 */
static UINT8 cdspStillEdgeScale;    /* 0x21d3 */
static UINT8 cdspStillEdgeLowGain;  /* 0x21d4 */
static UINT8 cdspStillEdgeHighGain; /* 0x21d5 */
static UINT8 cdspStillEdgeLowThr;   /* 0x21d6 */
static UINT8 cdspStillEdgeHighThr;  /* 0x21d7 */
static UINT8 cdspStillEdgeYhtbw;    /* 0x21d8 */

static UINT8 cdspStillEdgeFtr0;	    /* 0x21d9 */	
static UINT8 cdspStillEdgeFtr1;	    /* 0x21de */		
static UINT8 cdspStillEdgeFtr2;		/* 0x21dd */	
static UINT8 cdspStillEdgeFtr3;		/* 0x21dc */	
static UINT8 cdspStillEdgeFtr4;		/* 0x21db */	
static UINT8 cdspStillEdgeFtr5;		/* 0x21da */	

static UINT8 cdspStillIntLowThr;	/* 0x216c */

static UINT8 cdspStillYCoring[17];	/* 0x22a0 ~ 0x22b0 */ 


static UINT8 cdspPvFilterEn;		/* 0x21d0 */

static UINT8 cdspPvEdgeClamp;   	/* 0x21d2 */
static UINT8 cdspPvEdgeScale;   	/* 0x21d3 */
static UINT8 cdspPvEdgeLowGain; 	/* 0x21d4 */
static UINT8 cdspPvEdgeHighGain;	/* 0x21d5 */
static UINT8 cdspPvEdgeLowThr;  	/* 0x21d6 */
static UINT8 cdspPvEdgeHighThr; 	/* 0x21d7 */
static UINT8 cdspPvEdgeYhtbw;   	/* 0x21d8 */

static UINT8 cdspPvEdgeFtr0;		/* 0x21d9 */	
static UINT8 cdspPvEdgeFtr1;		/* 0x21de */	
static UINT8 cdspPvEdgeFtr2;		/* 0x21dd */	
static UINT8 cdspPvEdgeFtr3;		/* 0x21dc */	
static UINT8 cdspPvEdgeFtr4;		/* 0x21db */	
static UINT8 cdspPvEdgeFtr5;		/* 0x21da */	

static UINT8 cdspPvMedmode;			/* 0x21df */
static UINT8 cdspPvIntLowThr;		/* 0x216c */


static UINT8 cdspWBOffsetEnable;
static UINT8 cdspWBGainEnable;
static UINT8 cdspGammaEnable;
static UINT8 cdspLscEnable;


//static UINT8 cdspYuvEdgeCoringF = 0;

//static UINT8 cdspRegLsc;
static UINT8 cdspRegHC;
UINT8 cdspRegSatHue;
static UINT8 cdspBPEnable;
static UINT8 cdspRegLsc;


static UINT8 cdspMclkSrc;
static UINT16 cdspMclkDiv;
static UINT8 cdspMclkPhase;
static UINT8 cdspMclkDrv;
static UINT8 cdspMclkSync;
static UINT8 cdspMclkGpio;
static UINT8 cdspPclkSrc;
static UINT16 cdspPclkDiv;
static UINT8 cdspPclkPhase;
static xdata UINT8 halCdspEffectYuvChange = 0;

SysDynamicEnhan SysDynamicValue;
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern xdata UINT8 snapBurstOpt;
extern xdata UINT8 snapBurstCnt;

extern xdata UINT32 snapShutterSoundAddr;
extern xdata UINT8 dynamicIqTbl[256];


/**************************************************************************
 *               I N T E R N A L    F U N C T I O N D E C L A R A T I O N S*
 **************************************************************************/

static UINT8 cdspSenClockSwitch( UINT8 opt );
static UINT8 cdspRawReDo( UINT32 rawDataAddr, UINT16 rawHSize, UINT16 rawVSize, UINT8 rawMode, UINT8 mode);
static UINT8 cdspYuvFilterCtrl( UINT8 mode, UINT8 opt );
 
//static void cdspYCoringCfg( void );
//static UINT8 cdspBlockCtrl( UINT8 mode, UINT8 stage );

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/


/**
 * @fn        void cdspInit(void)
 * @brief     initialize parameters of CDSP
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-7
 * @todo      N/A
 * @bug       N/A
*/
void
cdspInit(
	void
)
{

	
	XBYTE[REG_Cdsp_disgatedmclk]|=0x01;	/* disable lens shading gating clock*/
	XBYTE[REG_Cdsp_disgatedv1xclk_low]|=0x01; 

	/* Get parameters of cdsp.bin and they're for capture */	


	#if 0
		HAL_CdspEdgeCoringMedianEnGet(&cdspStillFilterEn);	
		HAL_CdspEdgeYHClampGet(&cdspStillEdgeClamp);
		HAL_CdspEdgeYHDivGet(&cdspStillEdgeScale);	
		HAL_CdspEdgeGainGet(&cdspStillEdgeLowGain,&cdspStillEdgeHighGain);	
		HAL_CdspEdgeThrGet(&cdspStillEdgeLowThr,&cdspStillEdgeHighThr);
		HAL_CdspEdgeBwGet(&cdspStillEdgeYhtbw);	
	
		HAL_CdspEdgeFilterGet(  &cdspStillEdgeFtr0,&cdspStillEdgeFtr5,
								&cdspStillEdgeFtr4,&cdspStillEdgeFtr3,
								&cdspStillEdgeFtr2,&cdspStillEdgeFtr1);
	#endif

	cdspPostProcessCtrl(1,0);		/* preview mode */
		

}


/**
 * @fn        void cdspPreProcessOff(UINT8 mode)
 * @brief     turn off CDSP pre-processing block, bad pixel/wb offset/gamma/lsc
 * @param     [in] mode
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-7
 * @todo      N/A
 * @bug       N/A
*/
void
cdspPreProcessOff(
	UINT8 rawMode,
	UINT8 preProcMode
)
{

	cdspLscEnable=HAL_CdspLscEnGet();
	cdspWBGainEnable=HAL_CdspWBGainEnGet();
	cdspWBOffsetEnable=HAL_CdspWBOffsetEnGet();
	cdspGammaEnable=HAL_CdspGammaEnGet();
	

	HAL_CdspBPEnSet(0);
	
	if (preProcMode != SP1K_SNAP_POST_WB){ 
		HAL_CdspWBOffsetEnSet( 0 );
	}

	if( rawMode != CDSP_SNAP_10RAW ) { /* 10 bit mode does not off WB and Gamma */
		HAL_CdspWBGainEnSet( 0 );
		HAL_CdspGammaEnSet( 0 );
	}

	//XBYTE[0x21fa]|=0x01;
	//XBYTE[0x21f8]|=0x01; /* disable lens shading gating clock*/

	//XBYTE[0x21fa]&=0xfe;
	//XBYTE[0x21f8]&=0xfe;
	
	
	#ifndef DOYUV_FROM_CORE
		HAL_CdspLscEnSet(0);	/* LSC must be off if process is from preprocess*/  /* it must be close on EOF */								
		HAL_CdspRstBlock(CDSPRstEDGE_LSC);						
	#elif defined(DARKSUB_RAWMUX)
		if (frontSnapMethodGet()==SNAP_MS_DARK){
			HAL_CdspLscEnSet(0);	/* LSC must be off if process is from preprocess*/	/* it must be close on EOF */								
			HAL_CdspRstBlock(CDSPRstEDGE_LSC);	
			//printf("lsc is off\n");		
		}
	#endif	
}

/**
 * @fn        void cdspPreProcessReload(void)
 * @brief     reload parameters of CDSP pre-processing block
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void
cdspPreProcessReload(
	void
)
{        
    #ifndef COMPARE_ANSWER 
    		
		//UINT16 prvRgain, prvBgain, prvGgain;  //will mark

		//AAA_AWBGainGet(&prvRgain, &prvBgain, &prvGgain);  //will mark // hlc 0319 add
		//HAL_CdspWBGainSet(prvRgain, prvGgain, prvBgain, prvGgain);  //will mark // hlc 0319 add
		//awbGainReset();//wangjg++ for wb gain set
		//cdspPrintf("reload cdspWBGainEnable=%bx,cdspGammaEnable=%bx\n",cdspWBGainEnable,cdspGammaEnable);
		
		HAL_CdspWBGainEnSet( cdspWBGainEnable );
		HAL_CdspWBOffsetEnSet( cdspWBOffsetEnable );
		HAL_CdspGammaEnSet( cdspGammaEnable );
		//printf("lsc=%bd\n",cdspLscEnable);
		HAL_CdspLscEnSet( cdspLscEnable&0x01 );

		HAL_CdspBPEnSet( 1 );
	#endif
    
}

/**
 * @fn        void cdspPostProcessOff(void)
 * @brief     turn off CDSP pre-processing block
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void
cdspPostProcessOff(
	void
)
{
	//cdspRegHC = XBYTE[ REG_Cdsp_huecorren ];
	//XBYTE[ REG_Cdsp_huecorren ] = 0x00;
	HAL_CdspRegGet(REG_Cdsp_huecorren,&cdspRegHC);
	HAL_CdspHueCorrectionEnSet(0);
	
	//cdspRegSatHue = XBYTE[ REG_Cdsp_YbYcen ];
	//XBYTE[ REG_Cdsp_YbYcen ] = 0x00;
	HAL_CdspRegGet(REG_Cdsp_YbYcen,&cdspRegSatHue);
	HAL_CdspRegSet(REG_Cdsp_YbYcen,0);
	
	//cdspRegLsc= XBYTE[ REG_Cdsp_LscChmode ];
	//XBYTE[ REG_Cdsp_LscChmode ] = 0x00;
}

/**
 * @fn        void cdspPostProcessReload(void)
 * @brief     reload parameters of CDSP post-processing block
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void
cdspPostProcessReload(
	void
)
{
	//XBYTE[ 0x2170 ] = cdspRegHC;	
	//XBYTE[ 0x21B0 ] = cdspRegSatHue;

	HAL_CdspHueCorrectionEnSet(cdspRegHC&0x01);
	HAL_CdspRegSet(REG_Cdsp_YbYcen,cdspRegSatHue);

	//XBYTE[ 0x2121 ] = cdspRegLsc;

}

/**
 * @fn        void cdspPostProcessCtrl(void)
 * @brief     reload parameters of post process
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void
cdspPostProcessCtrl(
	UINT8 mode,
	UINT8 opt
)
{

	#if defined(AMCAP_OPTION) || defined(ICAT_OPTION)
		if(!icatIsConnected()){
			if (mode==0){	/* capture */	
				cdspYuvFilterCtrl( 0 ,opt);
			}
			else{
				cdspYuvFilterCtrl( 1 ,opt);
			}		
		}				
	#else		

		#ifdef COMPARE_ANSWER 
			mode=mode;
			opt=opt;
		#else
			if (mode==0){	/* capture */	
				cdspYuvFilterCtrl( 0 ,opt);
			}
			else{
				cdspYuvFilterCtrl( 1 ,opt);
			}
		#endif	
	#endif

}


/**
 * @fn        void sensorTrigger(UINT32 rawBufAddr, UINT16 sensorHSize, UINT16 sensorVSize, UINT16 dzoomHSize, UINT16 dzoomVSize, UINT16 rawHSize, UINT16 rawVSize, UINT8 soundOpt, UINT8 rawMode)
 * @brief     Change sensor output from preview size to capture size and get capture size of raw data
 * @param     [in] rawBufAddr : the address for saving raw data in DRAM
 * @param     [in] sensorHSize : front horizontal size
 * @param     [in] sensorVSize : front vertical size
 * @param     [in] dzoomHSize : horiztontal size for digital zoom (it decides whether it crops front horizontal size)
 * @param     [in] dzoomVSize : vertical size for digital zoom (it decides whether it crops front vertical size)
 * @param     [in] rawHSize : raw horizton size for saving raw data
 * @param     [in] rawVSize : raw vertical size for saving raw data
 * @param     [in] snapHSize : jpeg horizontal size
 * @param     [in] soundOpt :
 *								0: no shutter sound
 *								1: shutter sound 
 * @param     [in] rawMode :
 *								0: 8  bits raw data
 *								1: 16 bits raw data
 *								2: YUV data
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void
sensorPanoramaTrigger(
	UINT8 soundOpt
)
{	
	HAL_FrontParaSet(HAL_FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);
	HAL_CdspVDramSkipWrSyncSet(0);
	// skip IMG data to dram
	HAL_CdspVDramSkipWrSet(CDSP_SKIP_IMG,0x00);
	// skip AE/AWB/EDGE data to dram
	HAL_CdspVDramSkipWrSet(CDSP_SKIP_AWBAEEDGE,0xff);

	XBYTE[REG_Cdsp_Cropen] = XBYTE[REG_Cdsp_Cropen] & (~0x20);
	HAL_CdspSyncModeSet(IMMEDIATE);
    XBYTE[REG_Cdsp_HRGBscaleen] = 0;

	XBYTE[REG_Cdsp_Cropen] = XBYTE[REG_Cdsp_Cropen] & (~0x03);

	HAL_CdspBPEnSet( 0 );	/*disable bad pixel cancellation*/

	HAL_GlobalCamModeSet(2);

	HAL_Cdsp2ndPathEnSet(0); /*turn off second path*/

	HAL_CdspVDramSkipWrSyncSet(1);

	if( soundOpt == 1 ) {
		//HAL_AudResume();
		sp1kAudioPlay(snapShutterSoundAddr);  
	}
}

void
sensorTrigger(
	UINT32 rawBufAddr,
	UINT16 sensorHSize,
	UINT16 sensorVSize,
	UINT16 dzoomHSize,
	UINT16 dzoomVSize,
	UINT16 rawHSize,
	UINT16 rawVSize,
	UINT16 snapHSize,
	UINT8 soundOpt,
	UINT8 rawMode
)
{
	UINT16 hOffset, vOffset;
	UINT8 snapFlowOpt,badPixelOpt;
	UINT8 snapChangeRawOpt;
	UINT8 reg21d0;	

	cdspPrintfCapSize("senHSize=%d,senVSize=%d\n",sensorHSize,sensorVSize);
	cdspPrintfCapSize("dzHSize=%d,dzVSize=%d\n",dzoomHSize,dzoomVSize);
	cdspPrintfCapSize("rawHSize=%d,rawVSize=%d\n",rawHSize,rawVSize);
	
    snapFlowOpt = (UINT8)snapParamGet( SP1K_SNAP_PARAM_DOFLOW_OPT );
    badPixelOpt = (UINT8)snapParamGet( SP1K_SNAP_PARAM_BAD_PIXEL_OPT );
    snapChangeRawOpt = (UINT8)snapParamGet( SP1K_SNAP_PARAM_CHANGERAW );
	
	frontLensCompensationSet(1);

	HAL_FrontParaSet(HAL_FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 0, 0, 0, 0, 1);

	

	HAL_CdspVDramSkipWrSyncSet(0);

	//printf("0x2276=%bx,0x2260=%bx\n",XBYTE[0x2276],XBYTE[0x2260]);
	if(snapFlowOpt==CDSP_DO_FRONT){
		HAL_CdspVDramSkipWrSet(CDSP_SKIP_IMG,0xff);	/* avoid CDSP1(VDRAM) writing data to wrong address */
	}
	else{
		HAL_CdspVDramSkipWrSet(CDSP_SKIP_IMG,0x00);
	}
	
	// skip AE/AWB/EDGE data to dram
	HAL_CdspVDramSkipWrSet(CDSP_SKIP_AWBAEEDGE,0xff);
	
	
	
    //#ifndef CDSP_SKIP_PREPORCESS_SET
	XBYTE[REG_Cdsp_Cropen] = XBYTE[REG_Cdsp_Cropen] & (~0x20);
	HAL_CdspSyncModeSet(IMMEDIATE);
    XBYTE[REG_Cdsp_HRGBscaleen] = 0;
    //#endif
    
	hOffset = ( sensorHSize - dzoomHSize ) >> 1;
	vOffset = ( sensorVSize - dzoomVSize ) >> 1;

	// save cdsp bad pixel
    //	XBYTE[ 0x2130 ] = 0x00;
	HAL_CdspBPEnSet( 0 );	/*disable bad pixel cancellation*/
	
	HAL_CdspDramVSizeOffsetSet(0,0,0,0);
	HAL_Cdsp1stPathWriteOffset(0,0);
    
	HAL_GlobalCamModeSet(2);

	HAL_DramRawFrmBufAddrSet(rawBufAddr);

	HAL_Cdsp2ndPathEnSet(0); /*turn off second path*/	
	
	if( rawMode == CDSP_SNAP_8RAW ) {      //will: raw 8 bits mode
		HAL_DramRawFrmBufAddrSet(rawBufAddr);
		HAL_DramRawFrmBufSizeSet(rawHSize/2, rawVSize);
				
		cdspSnapDramImgTypeSet( CDSP_SNAP_8RAW );

		
		HAL_DramDRAMDarkAddrSet(rawBufAddr);   /* dark sub buffer address */
		//HAL_DramDRAMDarkAddrSet(0x337700);   /* dark sub buffer address */		
		//printf("rawBufAddr=0x%lx\n",rawBufAddr);
		
		HAL_DramDRAMDarkSizeSet(rawHSize/2);   /* dark sub h width */
		HAL_CdspDarkTDSizeSet(rawHSize);    /* dark sub h width */
		#ifdef DARKSUB_RAWMUX
			HAL_CdspDarkCfg(1,0);
		#else
			HAL_CdspDarkCfg(0,0);
		#endif
	}
	else if( rawMode == CDSP_SNAP_10RAW ) { //will: raw 10 bits mode 
		HAL_DramRawFrmBufAddrSet(rawBufAddr);
		HAL_DramRawFrmBufSizeSet(rawHSize, rawVSize);		
		
		cdspSnapDramImgTypeSet( CDSP_SNAP_10RAW );
                                        //will: It can't do dark-sub in raw 10 bit mode 
	}
	else {
		HAL_Dram1stPathFrmBufAAddrSet(rawBufAddr);
		HAL_Dram1stPathFrmBufSizeSet( rawHSize, rawVSize );

		HAL_CdspFrmCtrlEnSet(0);
		HAL_CdspWriteBufIdxSet(0);

		cdspSnapDramImgTypeSet( CDSP_SNAP_YUV );

		/* if 0x21d0 is on, the line buffer limitation is 768 */
		HAL_CdspRegGet(REG_Cdsp_EdgeEn,&reg21d0);		
		HAL_CdspRegSet(REG_Cdsp_EdgeEn,0x00);		
		//XBYTE[ 0x21d0 ] = 0x00;
	}
	
	HAL_CdspRawImgSource(0);

	#ifndef PV_RAW_DATA
		if( rawMode < CDSP_SNAP_YUV ) {/* Raw mode */
			if( dzoomHSize > rawHSize ) {
				HAL_CdspHScaleRgbSet(1, 1, IMMEDIATE, dzoomHSize, rawHSize);
			}
			else {
				HAL_CdspHScaleRgbSet(0, 1, IMMEDIATE, dzoomHSize, rawHSize);
			}
	        HAL_CdspHScaleYuvSet( IMMEDIATE, dzoomHSize, rawHSize );   
			HAL_CdspVScaleYuvSet( IMMEDIATE, dzoomVSize, rawVSize );   
		} 
		else {							/* YUV mode */
			// YUV sensor output 2X H size
			HAL_CdspHScaleYuvSet( IMMEDIATE, dzoomHSize , rawHSize );

			// Vscale, line buffer limitation is 1280
			ASSERT( ( rawVSize == dzoomVSize ) || ( rawHSize <= CDSP_MAX_LINEBUF ), 0 ); 

			HAL_CdspVScaleYuvSet( 0, dzoomVSize, rawVSize );
		}
		
		frontSnapModeSet(frontDevActGet());

		// adjust size to align to 2
		dzoomHSize = (( dzoomHSize + 1 )>>1) <<1;     // H size must be alignment to 2. Hw limitation
		dzoomVSize = (( dzoomVSize + 1 )>>1) <<1;

		hOffset = (hOffset>>1)<<1;
		vOffset = (vOffset>>1)<<1;

		if( sensorHSize > dzoomHSize ) {
			
			if( rawMode < CDSP_SNAP_YUV ) {  /* RGB */
				HAL_CdspCropSet(1 ,1, 0, hOffset, dzoomHSize, vOffset, dzoomVSize);
			}
			else { 									/* YUV sensor output 2x H size */
				HAL_CdspCropSet(1 ,1, 0, hOffset << 1, dzoomHSize << 1, vOffset, dzoomVSize);
			}

			//if( rawMode == CDSP_SNAP_YUV ) {
			//	XBYTE[ 0x2121 ] = XBYTE[ 0x2121 ] | 0x20;
			//}
		}
		else {
			HAL_CdspCropSet(0 ,0, 0, 0, 0, 0, 0);
		}
	#endif

	HAL_CdspVDramSizeClamp(1, rawHSize, 0, rawVSize, 1);  /* VDram clamp size disable */

	
	if (snapChangeRawOpt == 0){
		
		if( snapBurstCnt == 0 ) {
			if (snapHSize > rawHSize){
				frontSnapShot(0, snapBurstOpt, 1,snapFlowOpt,1); /*jpeg scale up, so it needs to slow pixel clock*/
			}
			else{
				frontSnapShot(0, snapBurstOpt, 1,snapFlowOpt,0); 
			}
			#if 0
			if( snapFlowOpt == CDSP_DO_FRONT ){   
		    	frontSnapShot(0, snapBurstOpt, 1,snapFlowOpt); /* snap do front flow */
			}
			else {                   				/* snap normal flow , change preview mode to capture mode */
				if (badPixelOpt==SNAP_BP_REALTIME){
					frontSnapShot(0, snapBurstOpt, SENSOR_CAP_NORMAL_BPREALTIME,snapFlowOpt); /* snap do normal flow, bad pixel mode realtime */
				}
				else{
					frontSnapShot(0, snapBurstOpt,1,snapFlowOpt); /* snap do normal flow, bad pixel mode offline */
				}
			}
			#endif
		}
		else {   
			frontSnapShot(0, snapBurstOpt,0,snapFlowOpt,0);		
			
			#if 0
			if (snapFlowOpt == CDSP_DO_FRONT ){   /* snap do fornt flow and burst mode, it doesn't need to change preview to capture */
		    	frontSnapShot(0, snapBurstOpt, SENSOR_CAP_DOFRONT,snapFlowOpt);
		    }
			else{
				frontSnapShot(0, snapBurstOpt, SENSOR_CAP_NO_CHGMODE,snapFlowOpt);
			}
			#endif
		}
	}

	
	HAL_CdspRegBackup();

	if (snapFlowOpt == CDSP_DO_FRONT){      /* snap do front flow */
		HAL_CdspRawImgSource(0);
	}	
	else{
		HAL_CdspRawImgSource(1);
	}
	
    //HAL_CdspRstBlock(CDSPRstALL);
	
	if( soundOpt == 1 ) {
		//HAL_AudResume();
		sp1kAudioPlay(snapShutterSoundAddr);  
	}

	// disable crop and RGB H scale
	//HAL_CdspCropSet(0 ,0, 0, 0, 0, 0, 0);
	//HAL_CdspHScaleRgbSet(0, 1, 0, 0, 0);
	
	if( rawMode == CDSP_SNAP_YUV ){
		HAL_CdspRegSet(REG_Cdsp_EdgeEn,reg21d0);			
	}	
}


/**
 * @fn        void cdspDoBufInit(UINT32 rawBufAddr, UINT32 yuvBufAddr1, UINT32 yuvBufAddr2, UINT32 yuvBufAddr3, UINT32 yuvBufAddr4, UINT16 rawHSize, UINT16 rawVSize, UINT16 yuvHSize, UINT16 yuvLines, UINT8 abBufOpt, UINT8 rawMode)
 * @brief     buffer initialization for CDSP do
 * @param     [in] rawBufAddr : raw data buffer address
 * @param     [in] yuvBufAddr1 : MPG buffer source 0 (capture YUV buffer A)
 * @param     [in] yuvBufAddr2 : MPG buffer source 1 (capture YUV buffer B)
 * @param     [in] yuvBufAddr3 : MPG buffer source 2
 * @param     [in] yuvBufAddr4 : MPG buffer source 3
 * @param     [in] rawHSize : raw data horizontal size
 * @param     [in] rawVSize : raw data vertical size
 * @param     [in] yuvHSize : yuv data horizontal size
 * @param     [in] yuvVSize : yuv data horizontal size
 * @param     [in] abBufOpt :
 *								0: single YUV buffer
 *								0: double YUV buffer 
 * @param     [in] rawMode :
 *								0: 8  bits raw data
 *								1: 16 bits raw data
 *								2: YUV data 
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void
cdspDoBufInit(
	UINT32 rawBufAddr,
	UINT32 yuvBufAddr1,
	UINT32 yuvBufAddr2,
	UINT32 yuvBufAddr3,
	UINT32 yuvBufAddr4,
	UINT16 rawHSize,
	UINT16 rawVSize,
	UINT16 yuvHSize,
	UINT16 yuvVSize,
	UINT8 abBufOpt,
	UINT8 rawMode
)
{

  	HAL_DramRawFrmBufAddrSet(rawBufAddr);

  	if( rawMode == CDSP_SNAP_8RAW) {
		HAL_DramRawFrmBufSizeSet( rawHSize / 2, rawVSize );
	}
	else {
		HAL_DramRawFrmBufSizeSet( rawHSize, rawVSize );
	}

	HAL_Dram1stPathFrmBufAAddrSet(yuvBufAddr1);



	if (abBufOpt == SNAP_TWO_YUVBUF ) {
		HAL_Dram1stPathFrmBufSizeSet(yuvHSize, yuvVSize);
	}
	else {
		//HAL_Dram1stPathFrmBufSizeSet(yuvHSize, yuvVSize);
		HAL_Dram1stPathFrmBufSizeSet(yuvHSize, 2048);     /* it needs to set 2048 (hardware limination*/
		//HAL_Dram1stPathFrmBufSizeSet(yuvHSize, 1944);   /*will: it's for compare answer of FPGA*/
		//HAL_Dram1stPathFrmBufSizeSet(yuvHSize, 1024);   /*will: it's for compare answer of FPGA*/
		//HAL_Dram1stPathFrmBufSizeSet(yuvHSize, yuvVSize);/*will: it's for compare answer of FPGA*/
	}


	HAL_Dram1stPathFrmBufBAddrSet(yuvBufAddr2);
	HAL_Dram1stPathFrmBufCAddrSet(yuvBufAddr3);

  	if( rawMode == CDSP_SNAP_YUV) {
		HAL_Dram1stPathFrmBufDAddrSet( rawBufAddr );		
		//HAL_Dram1stPathFrmBufDAddrSet( yuvBufAddr1 );		

		HAL_DramDoYuvSrcHSizeSet(rawHSize);
	}
	else {
		HAL_Dram1stPathFrmBufDAddrSet( yuvBufAddr4 );
	}

	
	//cdspPostProcessCtrl(0);   

}


/**
 * @fn        void cdspSnapDramImgTypeSet(UINT8 imgMode)
 * @brief     Set the type of CDSP output
 * @param     [in] imgMode : 
 *								0: 8  bits raw data
 *								1: 16 bits raw data
 *								2: YUV data   
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void 
cdspSnapDramImgTypeSet(
	UINT8 imgMode
)
{

	HAL_CdspFrmCtrlEnSet(0); /*Frcen disable*/
	XBYTE[REG_Dram_MpgSb_CdspWRegIdx]  = 0x00;	 //CDSP Write A FB
	XBYTE[REG_Dram_MpgSb_RegIdx_En] |= 0x02;	 //Enable CDSP Write Register

	switch (imgMode){
		case CDSP_SNAP_8RAW: 
			//HAL_CdspVDramMode(1, 1);
			HAL_CdspVDramDataType(1,1,0);
		break;
		case CDSP_SNAP_10RAW: 
			//HAL_CdspVDramMode(2, 1);
			HAL_CdspVDramDataType(2,1,0);
		break;
		case CDSP_SNAP_YUV: 
	    	//HAL_CdspVDramMode(0, 1);
	    	HAL_CdspVDramDataType(0,1,0);
		break;		
		default:		
			cdspPrintf("CDSP image type error \n");		
		break;
	}
}

/**
 * @fn        UINT8 cdspPvParameterSet(void)
 * @brief     set parameters of Coring/VDRAM/DRAMV for preview
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
cdspPvParameterSet(
	void
)
{
	//HAL_CdspVDramMode(0,0);
	HAL_CdspVDramDataType(0,0,0);
	#if 1  /* it is called by sp1kPreviewSet(), so it cannot use cdspYuvFilterCtrl() */
			
		//HAL_CdspEdgeCoringMedianEnSet(0,1,0,1);
		//HAL_CdspEdgeCoringMedianEnSet( cdspPvYEdge, cdspPvLEdge, cdspPvYCoring, cdspPvUvMedia );
		HAL_CdspYEdgeEnSet(cdspPvFilterEn);
		HAL_CdspLEdgeEnSet(cdspPvFilterEn>>1);
		HAL_CdspYCoringEnSet(cdspPvFilterEn>>2);
		HAL_CdspUVMedianEnSet(cdspPvFilterEn>>3);

		HAL_CdspYuvFilterBufSet(0);
		HAL_CdspEdgeFilterSet(  cdspPvEdgeFtr0,cdspPvEdgeFtr5,
								cdspPvEdgeFtr4,cdspPvEdgeFtr3,
								cdspPvEdgeFtr2,cdspPvEdgeFtr1);
		
	#else
		cdspYuvFilterCtrl( 1 );
	#endif

	HAL_CdspDramVMirrorSet(0,0,0);
	HAL_CdspDramVSkippixelSet(0);

	HAL_CdspDramVSizeOffsetSet(0,0,0,0);

	//HAL_Cdsp1stPathWriteOffset(0,0);  /* will mark */

	return CDSP_TASK_SUCCESS;
}


/**
 * @fn        void cdspContrastEffectSet(void)
 * @brief     contrast effect
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void 
cdspContrastEffectSet(
	void
)
{
	if( halCdspEffectYuvChange == 0 ) {
		// save original parameter
		HAL_CdspEffectYuvNormalSave();
	}

	//HAL_CdspBriContEnSet( 1, 0, 0, 0 );
	HAL_CdspBriContEnSet(1);
	HAL_CdspYThrEnSet(0);
	HAL_CdspYRevrseEnSet(0);
	HAL_CdspYThrSet(0);
	
	HAL_CdspYBrightOffsetSet(0);
	HAL_CdspYContrastOffsetSet(0x40);

	HAL_CdspSatHueEnSet(1);
	HAL_CdspSatHueParaSet(0x20, 0x00, 0x40, 0x00, 0x00, 0x00 );

	halCdspEffectYuvChange = 1;


}

/**
 * @fn        void cdspBlackWhiteEffectSet(void)
 * @brief     black&white effect
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void 
cdspBlackWhiteEffectSet(
	void
)
{
	if( halCdspEffectYuvChange == 0 ) {
		// save original parameter
		HAL_CdspEffectYuvNormalSave();
	}

	//HAL_CdspBriContEnSet( 1, 0, 0, 0 );
	HAL_CdspBriContEnSet(1);
	HAL_CdspYThrEnSet(0);
	HAL_CdspYRevrseEnSet(0);
	HAL_CdspYThrSet(0);
	
	HAL_CdspYBrightOffsetSet(0);
	HAL_CdspYContrastOffsetSet(0x20);

	
	HAL_CdspSatHueEnSet(1);
	HAL_CdspSatHueParaSet( 0x20, 0x00, 0x00, 0x00, 0x00, 0x00 );

	halCdspEffectYuvChange = 1;

}

/**
 * @fn        void cdspColdEffectSet(void)
 * @brief     cold effect
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void 
cdspColdEffectSet(
	void
)
{
	if( halCdspEffectYuvChange == 0 ) {
		// save original parameter
		HAL_CdspEffectYuvNormalSave();
	}

	//HAL_CdspBriContEnSet( 1, 0, 0, 0 );
	HAL_CdspBriContEnSet(1);
	HAL_CdspYThrEnSet(0);
	HAL_CdspYRevrseEnSet(0);
	HAL_CdspYThrSet(0);
	
	HAL_CdspYBrightOffsetSet(0);
	HAL_CdspYContrastOffsetSet(0x20);

	
	HAL_CdspSatHueEnSet(1);
	HAL_CdspSatHueParaSet( 0x20, 0x00, 0x40, 0x00, 0x30, 0xc0 );

	halCdspEffectYuvChange = 1;

}

/**
 * @fn        void cdspWarmEffectSet(void)
 * @brief     warm effect
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void 
cdspWarmEffectSet(
	void
)
{
	if( halCdspEffectYuvChange == 0 ) {
		// save original parameter
		HAL_CdspEffectYuvNormalSave();
	}

	//HAL_CdspBriContEnSet( 1, 0, 0, 0 );	
	HAL_CdspBriContEnSet(1);
	HAL_CdspYThrEnSet(0);
	HAL_CdspYRevrseEnSet(0);
	HAL_CdspYThrSet(0);

	
	HAL_CdspYBrightOffsetSet(0);
	HAL_CdspYContrastOffsetSet(0x20);

	
	HAL_CdspSatHueEnSet(1);
	HAL_CdspSatHueParaSet( 0x20, 0x00, 0x40, 0x00, 0xf0, 0x18 );

	halCdspEffectYuvChange = 1;

}

/**
 * @fn        void cdspSunsetEffectSet(void)
 * @brief     sunset effect
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void 
cdspSunsetEffectSet(
	void
)
{
	if( halCdspEffectYuvChange == 0 ) {
		// save original parameter
		HAL_CdspEffectYuvNormalSave();
	}

	//HAL_CdspBriContEnSet( 1, 0, 0, 0 );	
	HAL_CdspBriContEnSet(1);
	HAL_CdspYThrEnSet(0);
	HAL_CdspYRevrseEnSet(0);
	HAL_CdspYThrSet(0);
	
	HAL_CdspYBrightOffsetSet(0);
	HAL_CdspYContrastOffsetSet(0x20);

	
	HAL_CdspSatHueEnSet(1);
	HAL_CdspSatHueParaSet( 0x20, 0x00, 0x40, 0x00, 0xe0, 0x0a );

	halCdspEffectYuvChange = 1;

}

/**
 * @fn        void cdspSepiaEffectSet(void)
 * @brief     sepia effect
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void 
cdspSepiaEffectSet(
	void
)
{
	if( halCdspEffectYuvChange == 0 ) {
		// save original parameter
		HAL_CdspEffectYuvNormalSave();
	}

	//HAL_CdspBriContEnSet( 1, 0, 0, 0 );	
	HAL_CdspBriContEnSet(1);
	HAL_CdspYThrEnSet(0);
	HAL_CdspYRevrseEnSet(0);
	HAL_CdspYThrSet(0);

	
	HAL_CdspYBrightOffsetSet(0);
	HAL_CdspYContrastOffsetSet(0x20);

	
	HAL_CdspSatHueEnSet(1);
	HAL_CdspSatHueParaSet( 0x20, 0x00, 0x00, 0x00, 0xd8, 0x18 );

	halCdspEffectYuvChange = 1;
}

/**
 * @fn        void cdspNegativeEffectSet(void)
 * @brief     negative effect
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void 
cdspNegativeEffectSet(
	void
)
{
	if( halCdspEffectYuvChange == 0 ) {
		// save original parameter
		HAL_CdspEffectYuvNormalSave();
	}

	//HAL_CdspBriContEnSet( 1, 0, 1, 0 );
	HAL_CdspBriContEnSet(1);
	HAL_CdspYThrEnSet(0);
	HAL_CdspYRevrseEnSet(1);
	HAL_CdspYThrSet(0);
	
	HAL_CdspYBrightOffsetSet(0);
	HAL_CdspYContrastOffsetSet(0x20);

	
	HAL_CdspSatHueEnSet(1);
	HAL_CdspSatHueParaSet( 0x20, 0x00, 0xC0, 0x00, 0, 0 );

	halCdspEffectYuvChange = 1;

}

/**
 * @fn        void cdspBinarizeEffectSet(void)
 * @brief     binarize effect
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void 
cdspBinarizeEffectSet(
	void
)
{
	if( halCdspEffectYuvChange == 0 ) {
		// save original parameter
		HAL_CdspEffectYuvNormalSave();
	}

	//HAL_CdspBriContEnSet( 1, 1, 0, 0x80 );	
	HAL_CdspBriContEnSet(1);
	HAL_CdspYThrEnSet(1);
	HAL_CdspYRevrseEnSet(0);
	HAL_CdspYThrSet(0x80);

	
	HAL_CdspYBrightOffsetSet(0);
	HAL_CdspYContrastOffsetSet(0x20);

	
	HAL_CdspSatHueEnSet(1);
	HAL_CdspSatHueParaSet( 0x20, 0x00, 0x00, 0x00, 0, 0 );

	halCdspEffectYuvChange = 1;

}

/**
 * @fn        void cdspRedEffectSet(void)
 * @brief     red effect
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void 
cdspRedEffectSet(
	void
)
{
	if( halCdspEffectYuvChange == 0 ) {
		// save original parameter
		HAL_CdspEffectYuvNormalSave();
	}

	//HAL_CdspBriContEnSet( 0, 0, 0, 0x00 );	
	HAL_CdspBriContEnSet(0);
	HAL_CdspYThrEnSet(0);
	HAL_CdspYRevrseEnSet(0);
	HAL_CdspYThrSet(0);
	
	HAL_CdspYBrightOffsetSet(0x20);
	HAL_CdspYContrastOffsetSet(0x20);

	
	HAL_CdspSatHueEnSet(1);
	HAL_CdspSatHueParaSet( 0x10, 0x00, 0x40, 0x00, 0x00, 0x40 );

	halCdspEffectYuvChange = 1;

}

/**
 * @fn        void cdspBlueEffectSet(void)
 * @brief     bule effect
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void 
cdspBlueEffectSet(
	void
)
{
	if( halCdspEffectYuvChange == 0 ) {
		// save original parameter
		HAL_CdspEffectYuvNormalSave();
	}

	//HAL_CdspBriContEnSet( 0, 0, 0, 0x00 );	
	HAL_CdspBriContEnSet(0);
	HAL_CdspYThrEnSet(0);
	HAL_CdspYRevrseEnSet(0);
	HAL_CdspYThrSet(0);
	
	HAL_CdspYBrightOffsetSet(0x20);
	HAL_CdspYContrastOffsetSet(0x20);

	
	HAL_CdspSatHueEnSet(1);
	HAL_CdspSatHueParaSet( 0x10, 0x00, 0x40, 0x00, 0x40, 0x00 );

	halCdspEffectYuvChange = 1;

}

/**
 * @fn        void cdspGreenEffectSet(void)
 * @brief     green effect
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void 
cdspGreenEffectSet(
	void
)
{
	if( halCdspEffectYuvChange == 0 ) {
		// save original parameter
		HAL_CdspEffectYuvNormalSave();
	}

	//HAL_CdspBriContEnSet( 0, 0, 0, 0x00 );
	HAL_CdspBriContEnSet(0);
	HAL_CdspYThrEnSet(0);
	HAL_CdspYRevrseEnSet(0);
	HAL_CdspYThrSet(0);
	
	HAL_CdspYBrightOffsetSet(0x20);
	HAL_CdspYContrastOffsetSet(0x20);

	
	HAL_CdspSatHueEnSet(1);
	HAL_CdspSatHueParaSet( 0x10, 0x00, 0x40, 0x00, 0xe0, 0xc0 );

	halCdspEffectYuvChange = 1;

}

/**
 * @fn        void cdspNormalEffectSet(void)
 * @brief     normal effect
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void 
cdspNormalEffectSet(
	void
)
{
	if( halCdspEffectYuvChange == 1 ) {
		HAL_CdspEffectYuvNormalResotre();
	}

	halCdspEffectYuvChange = 0;

}

/**
 * @fn        void cdspBlendEffectSet(UINT32 srcAddr, UINT32 dstAddr, UINT16 xSize, UINT16 ySize, UINT8 idx)
 * @brief     blend effect
 * @param     [in] srcAddr : YUV source address in DRAM
 * @param     [in] dstAddr : YUV destination address in DRAM
 * @param     [in] hSize : YUV source horizontal size
 * @param     [in] vSize : YUV source vertical size
 * @param     [in] idx : 0~255. it is bigger and the picture is brighter and clearer
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void 
cdspBlendEffectSet(
	UINT32 srcAddr,
	UINT32 dstAddr,
	UINT16 hSize,
	UINT16 vSize,
	UINT8 idx
)
{
	UINT8 reg21d0;
	//idx=idx;
	//reg21d0 = XBYTE[0x21d0];
	HAL_CdspRegGet(REG_Cdsp_EdgeEn,&reg21d0);
	
	//XBYTE[0x21d0] = 0;
	//HAL_CdspEdgeCoringMedianEnSet(0,0,0,0);
	HAL_CdspYEdgeEnSet(0);
	HAL_CdspLEdgeEnSet(0);
	HAL_CdspYCoringEnSet(0);
	HAL_CdspUVMedianEnSet(0);
		
	HAL_CdspYuvFilterBufSet(0);
	
	if( halCdspEffectYuvChange == 0 ) {
		// save original parameter
		HAL_CdspEffectYuvNormalSave();
	}


	//XBYTE[0x21b0] = XBYTE[0x21b0] | 0x11;
	HAL_CdspBriContEnSet(1);
	HAL_CdspSatHueEnSet(1);
	
	//XBYTE[0x21b4] = 0x80 + (((~idx)>>1) +1);//0xe0;//(UINT8)-30;  //Y value offset (-128~127) 2'complement
	HAL_CdspYBrightOffsetSet(0x80 + (((~idx)>>1) +1)); /*//Y value offset (-128~127) 2'complement*/

	//XBYTE[0x21b5] = ((~idx)>>3) + 1;//0x12; // contrast 3.5
	HAL_CdspYContrastOffsetSet(((~idx)>>3) + 1); /* contrast 3.5 */
	
	//XBYTE[0x21b6] = 0x20; // saturation 3.5
	halCdspEffectYuvChange = 1;

	cdspYuv2Yuv( srcAddr, 
				dstAddr, 
				hSize, 
				vSize,
				hSize,
				vSize,
				32UL);
	cdspNormalEffectSet();
	
	//XBYTE[0x21d0] = reg21d0;
	HAL_CdspRegSet(REG_Cdsp_EdgeEn,reg21d0);
}

/**
 * @fn        UINT8 cdspSenClockSwitch(UINT8 opt)
 * @brief     cdspSenClockSwitch
 * @param     [in] opt :
 *						0: save current clock/TG_GPIO register settings and set new settings
 *						1: restore old colck/TG_GPIO register settings
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
static UINT8
cdspSenClockSwitch(
	UINT8 opt
)
{
	if ( opt ) {  /* save current setting and set new setting. */
		HAL_FrontParaMClkCfgGet(&cdspMclkSrc, &cdspMclkDiv, &cdspMclkPhase, &cdspMclkDrv, &cdspMclkSync);
		HAL_FrontParaPClkCfgGet(&cdspPclkSrc, &cdspPclkDiv, &cdspPclkPhase);
		cdspMclkGpio = HAL_GpioByteFuncGet(HAL_GPIO_BYTE_TG0, 0x04);
		
		HAL_GpioByteFuncSet(HAL_GPIO_BYTE_TG0, 0x04, 0x04);
		HAL_FrontParaSet(HAL_FRONT_PARA_MCLK_CFG, 0, 2, 0, cdspMclkDrv, 0);
		HAL_FrontParaSet(HAL_FRONT_PARA_PCLK_CFG, 0, 1, 0, 0, 0);
	}
	else {  /* restore old setting. */
		HAL_FrontParaSet(HAL_FRONT_PARA_MCLK_CFG, cdspMclkSrc, cdspMclkDiv, cdspMclkPhase, cdspMclkDrv, cdspMclkSync);
		HAL_FrontParaSet(HAL_FRONT_PARA_PCLK_CFG, cdspPclkSrc, cdspPclkDiv, cdspPclkPhase, 0, 0);

		HAL_GpioByteFuncSet(HAL_GPIO_BYTE_TG0, 0x04, cdspMclkGpio);
	}

	return CDSP_TASK_SUCCESS;
}

/**
 * @fn        UINT8 cdspRawPostWB(UINT32 rawDataAddr, UINT16 rawHSize, UINT16 rawVSize, UINT8 rawMode)
 * @brief     cdsp post white balance 
 * @param     [in] rawDataAddr : raw data address in DRAM
 * @param     [in] rawHSize : raw horizontal size
 * @param     [in] rawVSize : raw vertical size
 * @param     [in] rawMode : 
 *								0: 8  bits raw data
 *								1: 16 bits raw data
 *								2: YUV data  
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
UINT8
cdspRawProcess(
	UINT32 rawDataAddr,
	UINT16 rawHSize,
	UINT16 rawVSize,
	UINT8 rawMode,
	UINT8 mode
)
{
	UINT8 awbMode=0,awbProc=0;	/*wangjg++ for post wb */

	if (mode==SP1K_SNAP_POST_WB){
		//printf("post WB\n");
		cdspRawReDo( rawDataAddr, rawHSize, rawVSize, rawMode , SP1K_SNAP_GET_WBGAIN);
		awbPostProc();
	}
	
	cdspRawReDo( rawDataAddr, rawHSize, rawVSize, rawMode , mode);



	#ifndef CDSP_TASK_TAG_TODO
		//sp1kAwbModeSet(AWB_MODE_Flash);
		//awbSvcProc(NULL);
		/*
			awbMode = sp1kAwbParamSet(AWB_PARA_Mode, AWB_MODE_Flash);
			awbProc = sp1kAwbParamSet(AWB_PARA_Proc, AWB_PROC_Snap);
			awbDefaultProc();   
			awbMode = sp1kAwbParamSet(AWB_PARA_Mode, awbMode);
			awbProc = sp1kAwbParamSet(AWB_PARA_Proc, awbProc);
		*/
	#endif
	
	return SUCCESS;
}

UINT8
cdspRawBadPixel(
	UINT32 rawDataAddr,
	UINT16 rawHSize,
	UINT8 rawMode
)
{

	//UINT8 reg2e01;

	//ret = sp1kBpFix( rawDataAddr, rawXSize, rawYSize );  //old bad pixel function
	//HAL_CdspStillBpTblAddrSet(K_SDRAM_BadPixelCoordBufAddr + K_SDRAM_BadPixel_PrvBufSize);
	
	//for (delay=0;delay<0xffff;delay++){;}

	if (rawMode != CDSP_SNAP_8RAW){
	 	return CDSP_TASK_FAIL;
	}
	
	HAL_MoeReset();
	HAL_MoeEnable();
	
	HAL_CdspStillBpCfg(0,0,0,rawHSize);
	HAL_CdspStillBpTblAddrSet(K_SDRAM_BadPixelCoordBufAddr + K_SDRAM_BadPixel_PrvBufSize);		  
	HAL_CdspStillBpRAWAddrSet(rawDataAddr);  /*will: Bad pixel source address*/
	HAL_CdspStillBpEnSet(1);
	//wangjg++ for moe 
	//moeDbgTrig = MOE_Trigger_Bp;
	HAL_MoeRunCmd(NULL,MOE_Trigger_Bp);
	
	cdspPrintf("suc up bp \n");
	
	#if 1	/*compensate lower part of picture */ 
		HAL_CdspStillBpTblAddrSet(K_SDRAM_BadPixelCoordBufAddr + K_SDRAM_BadPixel_PrvBufSize + /*((UINT32)1024 * 8)*/K_SDRAM_BadPixel_CapBufSize_Upper);	//4 //fan.xiao modified 2011-10-25 , 17:02:25	  
		HAL_CdspStillBpEnSet(1);
		HAL_MoeRunCmd(NULL,MOE_Trigger_Bp);
		cdspPrintf("suc low bp \n");
	#endif
/*	
	//HAL_MoeSetMode(0);		
	reg2e01=XBYTE[0x2e01];
	HAL_MoeSetIntrEnable(0,0,0,0);
	
	XBYTE[0x2E00] = 0xFF;		
	HAL_CdspStillBpTrigger(1);
	
	//printf("K BP start type 1\n");
	while (( (XBYTE[0x2E00]&0x02) == 0x00) && (!XBYTE[0x2e02])){;}		
	//printf("K BP finish type 1\n");
	XBYTE[0x2e01]=reg2e01;
*/
	return SUCCESS;
}
/**
 * @fn        UINT8 cdspDoRawWB(UINT32 rawDataAddr, UINT16 rawXSize, UINT16 rawYSize, UINT8 rawMode)
 * @brief     cdsp do post white balance 
 * @param     [in] rawDataAddr : raw data address in DRAM
 * @param     [in] rawHSize : raw horizontal size
 * @param     [in] rawVSize : raw vertical size
 * @param     [in] rawMode : 
 *								0: 8  bits raw data
 *								1: 16 bits raw data
 *								2: YUV data  
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
static UINT8
cdspRawReDo(
	UINT32 rawDataAddr,
	UINT16 rawHSize,
	UINT16 rawVSize,
	UINT8 rawMode,
	UINT8 mode
)
{

	#if 1
		UINT8 reg2114,reg2121,reg2138,reg2150,reg2160;
		UINT8 val;
		
		reg2114=HAL_CdspWBOffsetEnGet(); /* WB Offset */
		reg2150=HAL_CdspWBGainEnGet();   /* WB Gain */
		reg2160=HAL_CdspGammaEnGet();    /* Gamma */		
		HAL_CdspRegGet(REG_Cdsp_LscChmode,&reg2121); /* LSC */  
		HAL_CdspRegGet(REG_Cdsp_Cropen,&reg2138); /* image crop */

		
		HAL_CdspClockSet(1,7);	

		HAL_CdspDramVInterval(0x0070);

	  	if( rawMode == CDSP_SNAP_8RAW) {
			HAL_CdspVDramDataType( 1, 1, 0);  /* From LUT Gamma */		
		}
		else {
			HAL_CdspVDramDataType( 2, 1, 0);  /* From WB Gain */
		}

		
		//HAL_CdspVDramSkipWrSyncSet(0);
		//HAL_CdspVDramSkipAeWrSet(1);
		//HAL_CdspVDramSkipDistEdgeWrSet(1);
		HAL_CdspVDramSkipWrGet(&val);		
		HAL_CdspVDramSkipWrSyncSet(0);		
		HAL_CdspVDramSkipWrSet(CDSP_SKIP_AEEDGE,0xff);

		//HAL_CdspWBOffsetSet(1,0,30,0,30,0,1);  /* WB Offset for test */

		
		#ifndef COMPARE_ANSWER							
			switch (mode){
				case SP1K_SNAP_PRE_REDO_DARKSUB:
					HAL_CdspWBOffsetEnSet(0);				/* turn off WB Offset */	
					HAL_CdspWBGainEnSet(1); 				/* turn on WB Gain */
					HAL_CdspGammaEnSet(1);					/* trun on Gamma */
					HAL_CdspLscEnSet(1);					/* turn on LSC */ /* It must be set on EOF */					
				break;
				case SP1K_SNAP_POST_WB:		
					HAL_CdspWBOffsetCfgSet(0,1);			/* sync immediately */
					HAL_CdspWBOffsetEnSet(0);				/* turn off WB Offset */	
					HAL_CdspWBGainEnSet(cdspWBGainEnable); 	/* turn on WB Gain */
					HAL_CdspGammaEnSet(cdspGammaEnable);	/* trun on/off Gamma */
					HAL_CdspLscEnSet(0);	/* turn off LSC */ /* It must be set on EOF */					
					//HAL_CdspWBGainSet(0x100,0x40,0x80,0x40);	/*test wb gain */								
					HAL_CdspWBOffsetCfgSet(1,1);			/* sync with VD */
					cdspPrintf("post wb\n");
				break;
				case SP1K_SNAP_GET_WBGAIN:
					HAL_CdspWBOffsetCfgSet(0,1);			/* sync immediately */
					HAL_CdspWBOffsetEnSet(0);				/* turn off WB Offset */	
					HAL_CdspWBGainEnSet(0); 				/* turn off WB Gain */
					HAL_CdspGammaEnSet(0);					/* trun off Gamma */
					HAL_CdspLscEnSet(0);					/* turn off LSC */ /* It must be set on EOF */										
					HAL_CdspWBOffsetCfgSet(1,1);			/* sync with VD */
					
					HAL_CdspAWBWinEn( 1, 0, 0, 0 ,0);		
					HAL_CdspAWBWinSet(rawHSize,rawVSize);		/* set AWB Window */	
					HAL_CdspVDramSkipWrSyncSet(0);				/* sync immediate*/		
					HAL_CdspVDramSkipWrSet(CDSP_SKIP_AWB,0xfd);	/* don't skip AWB data  */ 
				break;
				case SP1K_SNAP_PRE_REDO:
				default:
				break;
			}
		#endif

		
		HAL_CdspRegSet(REG_Cdsp_Cropen,reg2138&0xfc); /* turn off image crop */
		
		//HAL_CdspWBOffsetEnSet(1);    				  /* test if "raw to raw" can work */	
		//XBYTE[0x2118]=0x02;
		
		
		HAL_CdspSyncModeSet(IMMEDIATE);
		
		XBYTE[ REG_Cdsp_HRGBscaleen ] = 0x00;		/* disable RGB H Scale */

		HAL_CdspVDramSizeClamp(1, rawHSize, 0, rawVSize, 1);  /*will: VDram clamp size disable*/

		
		XBYTE[REG_Cdsp_mirrorline] = 0x00;
		if (HAL_CdspLscEnGet()==0x01){   /* dummy line is on when LSC is on */
			XBYTE[REG_Cdsp_mirrorline] |= 0x80;				
		}
		
		XBYTE[REG_Cdsp_Skippixel] = 0x00;
		XBYTE[REG_Cdsp_Rhsize_low] = (UINT8)(rawHSize & 0xff);
		XBYTE[REG_Cdsp_Rhsize_high] = (UINT8)(rawHSize>>8);
		XBYTE[REG_Cdsp_Rhoffset_low] = 0x00;
		XBYTE[REG_Cdsp_Rhoffset_high] = 0x00;
		XBYTE[REG_Cdsp_Rvsize_low] = (UINT8)(rawVSize & 0xff);
		XBYTE[REG_Cdsp_Rvsize_high] = (UINT8)(rawVSize >>8);
		XBYTE[REG_Cdsp_Rvoffset_low] = 0x00;
		XBYTE[REG_Cdsp_Rvoffset_high] = 0x00;

		XBYTE[REG_Cdsp_Whoffset_low] = 0x00;
		XBYTE[REG_Cdsp_Whoffset_high] = 0x00;
		XBYTE[REG_Cdsp_Wvoffset_low] = 0x00;	
		XBYTE[REG_Cdsp_Wvoffset_high] = 0x00;	
		XBYTE[REG_Cdsp_Clamphsize_low] = (UINT8)(rawHSize & 0xff);;
		XBYTE[REG_Cdsp_Clamphsize_high] = (UINT8)(rawHSize>>8);
		

	  	HAL_DramRawFrmBufAddrSet(rawDataAddr);

		if( rawMode == CDSP_SNAP_8RAW) {
			HAL_DramRawFrmBufSizeSet( rawHSize >>1, rawVSize );
		}
		else {
			HAL_DramRawFrmBufSizeSet( rawHSize, rawVSize );
		}

		//XBYTE[ REG_Cdsp_evnt1 ] = XBYTE[ REG_Cdsp_evnt1 ] & ~0x08;


		XBYTE[REG_Dram_MpgSb_RegIdx_En] = XBYTE[REG_Dram_MpgSb_RegIdx_En] | 0x02;	/* CDSP write width idx*/
		XBYTE[REG_Dram_MpgSb_CdspWRegIdx] = 0x00;					/* CDSP clear write idx*/
		XBYTE[REG_Dram_TargetYuv] = 0x01;							/* clear CDSP skip fram */	   
		XBYTE[REG_Dram_MpgSb_CdspWRegIdx]|= 0x08;					/* turn off frame rate control*/


		XBYTE[ REG_Cdsp_evnt1 ] = 0x02;			 /* Clear Do cdsp interrupt */
		XBYTE[ REG_Cdsp_Remode ] |= 0x10 ;        /* Trigger Do CDSP RGB */

		while ((XBYTE[REG_Cdsp_evnt1]&0x02) == 0x00){
			cdspPrintf("Wait CDSP Done\n");
		}

		
		HAL_CdspWBOffsetEnSet(reg2114);					  /* restore WB Offset */	
		HAL_CdspWBGainEnSet(reg2150); 					  /* restore WB Gain */
		HAL_CdspGammaEnSet(reg2160);						  /* restore Gamma */
		HAL_CdspLscEnSet((reg2121&0x10)>>4);						  /* restore LSC */
		HAL_CdspRegSet(REG_Cdsp_Cropen,reg2138); /* turn off image crop */

		HAL_CdspVDramSkipWrSet(CDSP_SKIP_ALL,val);
		HAL_CdspVDramSkipWrSyncSet(val>>4);

	#else
		UINT32 hF;
		UINT16 initF;
		UINT16 doXSize, doYSize, awbXSize, awbYSize;
		UINT16 awbHF, awbVF, i;
		UINT16 shiftVLine;
		UINT32 rawShiftSize, awbShiftSize; // WORD size
		UINT32 rAddr, awbAddr;
		UINT16 wbVLine = CDSP_POST_WB_V_SIZE / CDSP_POST_WB_EACH_V_SIZE;

		UINT8 reg2114 = XBYTE[ 0x2114 ];  /* wb offset */
		UINT8 reg2121 = XBYTE[ 0x2121 ];  /* LSC */
		UINT8 reg2130 = XBYTE[ 0x2130 ];  /* preview bad pixel */  
		UINT8 reg2138 = XBYTE[ REG_Cdsp_Cropen ];
		UINT8 reg2150 = XBYTE[ REG_Cdsp_gain_en ];

		cdspSenClockSwitch( 1 );

		shiftVLine = ( rawYSize + wbVLine - 1 ) / wbVLine;

	    XBYTE[0x226E]  =  0x88;
	    XBYTE[0x226f]  =  0x01; // pixel clock between line interval

		ASSERT( rawMode < CDSP_SNAP_YUV, 0 );

	  	if( rawMode == CDSP_SNAP_8RAW) {
		    HAL_CdspVDramMode( 1, 0 );
		}
		else {
		    HAL_CdspVDramMode( 2, 0 );
		}

		XBYTE[0x2114] = 0x00;
		XBYTE[0x2121] = XBYTE[0x2121] & ~0x10;
		XBYTE[0x2130] = 0x00;
		XBYTE[REG_Cdsp_Cropen] = 0x00;
		XBYTE[REG_Cdsp_gain_en] = 0x00;

		// RGB scale
		if( rawXSize > 1024 ) {
			hF = ( (UINT32)1024 << 16 ) / ( rawXSize - 1 );

			initF = 65536 - ( ( hF * rawXSize ) & 0xffff );

			XBYTE[ REG_Cdsp_YUVscalesvden ] = 0x00;
			XBYTE[ 0x2145 ] = 0x11;

			XBYTE[ REG_Cdsp_HRGBscaleinieve_low ] = XBYTE[ REG_Cdsp_HRGBscaleiniodd_low ] = XBYTE[ REG_Cdsp_HRGBscalefactor_low ] = *((UINT8 *)&hF + 3 );
			XBYTE[ REG_Cdsp_HRGBscaleinieve_high ] = XBYTE[ 0x214b ] = XBYTE[ REG_Cdsp_HRGBscalefactor_high ] = *((UINT8 *)&hF + 2 );

			awbXSize = 1024;
		}
		else {
			XBYTE[ REG_Cdsp_YUVscalesvden ] = 0x00;
			XBYTE[ 0x2145 ] = 0x00;

			awbXSize = rawXSize;
		}
		awbYSize = CDSP_POST_WB_EACH_V_SIZE << 1;

		doXSize = rawXSize;
		doYSize = awbYSize;

		// AWB window set
		HAL_CdspAWBWinScaleEn( 0, 0 );

		awbHF = ( UINT16 ) ( ( ( UINT32 ) CDSP_POST_WB_H_SIZE << 16 ) / awbXSize );
		awbVF = ( UINT16 ) ( ( ( UINT32 ) CDSP_POST_WB_EACH_V_SIZE << 16 ) / awbYSize );

		HAL_CdspAWBWincfg( 0, awbXSize - 1, 0, awbYSize - 1, awbHF, awbVF );

		HAL_CdspAWBWinScaleEn( 1, 1 );

		HAL_CdspAWBWinEn( 1, 0, 0, 0 ,0); /* unhold window */

		HAL_Cdsp1stPathWriteOffset(0,0);
		HAL_CdspVDramSizeClamp( 0, 0, 0, 0, 0 );

		XBYTE[0x2264] = 0x00;
		XBYTE[0x2265] = 0x00;
		XBYTE[0x2266] = *((UINT8 *)&doXSize + 1 );
		XBYTE[0x2267] = *((UINT8 *)&doXSize );
		XBYTE[0x2268] = 0x00;
		XBYTE[0x2269] = 0x00;
		XBYTE[0x226a] = *((UINT8 *)&doYSize + 1 );
		XBYTE[0x226b] = *((UINT8 *)&doYSize );
		XBYTE[0x226c] = 0x00;
		XBYTE[0x226d] = 0x00;

		HAL_CdspVDramSkipWr( 5, 0 );

		if( rawMode == CDSP_SNAP_8RAW) {
			HAL_DramRawFrmBufSizeSet( doXSize / 2, doYSize );
			rawShiftSize = (UINT32)shiftVLine * doXSize >> 1;
		}
		else {
			HAL_DramRawFrmBufSizeSet( doXSize, doYSize );
			rawShiftSize = (UINT32)shiftVLine * doXSize;
		}

		awbShiftSize = ( (UINT32)CDSP_POST_WB_H_SIZE * CDSP_POST_WB_EACH_V_SIZE ) >> 1; // WORD size

		rAddr = rawDataAddr;
		awbAddr = K_SDRAM_PvAwbWndAddr;

		XBYTE[ REG_Cdsp_evnt1 ] = XBYTE[ REG_Cdsp_evnt1 ] & ~0x08;

		for( i = 0; i < wbVLine; i++ ) {
		  	HAL_DramRawFrmBufAddrSet( rAddr );
			HAL_CdspAWBwinAddrset( awbAddr );

			XBYTE[ 0x2260 ] = 0x10 | ( rawMode & 0x03 );   /* Set DoCDSP = 1 */

			while( ( XBYTE[ 0x2260 ] & 0x10 ) == 0x10 );

			rAddr = rAddr + rawShiftSize;
			awbAddr = awbAddr + awbShiftSize;
		}

		cdspSenClockSwitch( 0 );

		HAL_CdspVDramSkipWr( 6, 0 );

		// set awb address to default
		HAL_CdspAWBwinAddrset( K_SDRAM_PvAwbWndAddr );

		XBYTE[ 0x2114 ] = reg2114;
		XBYTE[ 0x2121 ] = reg2121;
		XBYTE[ 0x2130 ] = reg2130;
		XBYTE[ REG_Cdsp_Cropen ] = reg2138;
		XBYTE[ REG_Cdsp_gain_en ] = reg2150;
	#endif

	return CDSP_TASK_SUCCESS;
	
}

/**
 * @fn        UINT8 cdspYuvFilterCtrl(UINT8 enable)
 * @brief     Set Y coring, Y Edge and UV median for preview/capture mode
 * @param     [in] mode :
 *							0: capture mode
 *							1: preview mode 
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/

static UINT8
cdspYuvFilterCtrl(
	UINT8 mode,
	UINT8 opt
)
{
	if( mode == 0 ) {	/* capture mode */
		//printf("set 0x21d0\n");
		if(osSysModeGet() != OS_SYS_MODE_STILL_PB)//conflict with pb
		{
			if (SysDynamicValue.stillDynamicEn == CDSP_DYNAMIC_ENABLE) {
				cdspStillDynamicEnhanFlow();
			}
		}
		if ((opt==CDSP_DO_QV)||(opt==CDSP_DO_YUV_QV)){
			HAL_CdspYEdgeEnSet(0);
			HAL_CdspLEdgeEnSet(cdspStillFilterEn>>1);
			HAL_CdspYCoringEnSet(cdspStillFilterEn>>2);
			HAL_CdspUVMedianEnSet(cdspStillFilterEn>>3);				
		}
		else if(opt==CDSP_DO_YUV2YUV){
			HAL_CdspYEdgeEnSet(0);
			HAL_CdspLEdgeEnSet(0);
			HAL_CdspYCoringEnSet(0);
			HAL_CdspUVMedianEnSet(0);				
		}
		else{
			HAL_CdspYEdgeEnSet(cdspStillFilterEn);
			HAL_CdspLEdgeEnSet(cdspStillFilterEn>>1);
			HAL_CdspYCoringEnSet(cdspStillFilterEn>>2);
			HAL_CdspUVMedianEnSet(cdspStillFilterEn>>3);				
		}
			
		HAL_CdspYuvFilterBufSet(1);		/*capture*/
		HAL_CdspEdgeYHClampSet(cdspStillEdgeClamp);
		HAL_CdspEdgeYHDivSet(cdspStillEdgeScale);
		HAL_CdspEdgeGainSet(cdspStillEdgeLowGain,cdspStillEdgeHighGain);
		HAL_CdspEdgeThrSet(cdspStillEdgeLowThr,cdspStillEdgeHighThr);
		HAL_CdspEdgeBwSet(cdspStillEdgeYhtbw);

		HAL_CdspIntplLowThrSet(cdspStillIntLowThr);

		HAL_CdspEdgeFilterSet(cdspStillEdgeFtr0,cdspStillEdgeFtr5,
								cdspStillEdgeFtr4,cdspStillEdgeFtr3,
								cdspStillEdgeFtr2,cdspStillEdgeFtr1);

		HAL_CdspYCoringCoefficientSet(&cdspStillYCoring[0]);


		//cdspPrintf("filter=0x%bx\n",cdspStillFilterEn);
		cdspPrintf("s 21d0=0x%bx\n",XBYTE[REG_Cdsp_EdgeEn]);
		cdspPrintf("s 21d5=0x%bx\n",XBYTE[REG_Cdsp_ygain1]);
		cdspPrintf("s 21d9=0x%bx\n",XBYTE[REG_Cdsp_ftr0]);
		//cdspPrintf("21d8=0x%bx\n",XBYTE[0x21d8]);
			
	}
	else {	/* preview mode */

		cdspPvDynamicParaFullLoad(0x3c);
			
		//HAL_CdspEdgeCoringMedianEnSet( cdspPvYEdge, cdspPvLEdge, cdspPvYCoring, cdspPvUvMedia );
		HAL_CdspYEdgeEnSet(cdspPvFilterEn);
		HAL_CdspLEdgeEnSet(cdspPvFilterEn>>1);
		HAL_CdspYCoringEnSet(cdspPvFilterEn>>2);
		HAL_CdspUVMedianEnSet(cdspPvFilterEn>>3);
				
			
		HAL_CdspYuvFilterBufSet(0);

		HAL_CdspEdgeYHClampSet(cdspPvEdgeClamp);
		HAL_CdspEdgeYHDivSet(cdspPvEdgeScale);	
		HAL_CdspEdgeGainSet(cdspPvEdgeLowGain,cdspPvEdgeHighGain);	
		HAL_CdspEdgeThrSet(cdspPvEdgeLowThr,cdspPvEdgeHighThr);
		HAL_CdspEdgeBwSet(cdspPvEdgeYhtbw);

		HAL_CdspIntplLowThrSet(cdspPvIntLowThr);
		HAL_CdspMedianModeSet(cdspPvMedmode);
			
		HAL_CdspEdgeFilterSet(  cdspPvEdgeFtr0,cdspPvEdgeFtr5,
								cdspPvEdgeFtr4,cdspPvEdgeFtr3,
								cdspPvEdgeFtr2,cdspPvEdgeFtr1);

		//HAL_CdspEdgeThrSet(cdspPvEdgeLowThr,cdspPvEdgeHighThr);
		//HAL_CdspEdgeYHClampSet(cdspPvEdgeClamp);
		cdspPrintf("pv 21d0=0x%bx\n",XBYTE[REG_Cdsp_EdgeEn]);
		cdspPrintf("pv 21d5=0x%bx\n",XBYTE[REG_Cdsp_ygain1]);
		cdspPrintf("pv 21d9=0x%bx\n",XBYTE[REG_Cdsp_ftr0]);
			
	}

	return SUCCESS;
}

void
cdspPvEdgeFilterSet(
	UINT8 ftr0,
	UINT8 ftr1,
	UINT8 ftr2,
	UINT8 ftr3,
	UINT8 ftr4,
	UINT8 ftr5
)
{
	cdspPvEdgeFtr0=ftr0;
	cdspPvEdgeFtr1=ftr1;
	cdspPvEdgeFtr2=ftr2;
	cdspPvEdgeFtr3=ftr3;
	cdspPvEdgeFtr4=ftr4;
	cdspPvEdgeFtr5=ftr5;
}

void
cdspIntrPvEdgeFilterSet(
	UINT8 ftr0,
	UINT8 ftr1,
	UINT8 ftr2,
	UINT8 ftr3,
	UINT8 ftr4,
	UINT8 ftr5
)
{
	cdspPvEdgeFtr0=ftr0;
	cdspPvEdgeFtr1=ftr1;
	cdspPvEdgeFtr2=ftr2;
	cdspPvEdgeFtr3=ftr3;
	cdspPvEdgeFtr4=ftr4;
	cdspPvEdgeFtr5=ftr5;
}



void
cdspPvEdgeParaSet(
	UINT8 clamp,
	UINT8 div,
	UINT8 lowGain,
	UINT8 highGain,
	UINT8 lowThr,
	UINT8 highThr,
	UINT8 yhtbw
)
{

	cdspPvEdgeClamp=clamp;
	cdspPvEdgeScale=div;
	cdspPvEdgeLowGain=lowGain;
	cdspPvEdgeHighGain=highGain;
	cdspPvEdgeLowThr=lowThr;
	cdspPvEdgeHighThr=highThr;
	cdspPvEdgeYhtbw=yhtbw;
}

void
cdspIntrPvEdgeParaSet(
	UINT8 clamp,
	UINT8 div,
	UINT8 lowGain,
	UINT8 highGain,
	UINT8 lowThr,
	UINT8 highThr,
	UINT8 yhtbw
)
{

	cdspPvEdgeClamp=clamp;
	cdspPvEdgeScale=div;
	cdspPvEdgeLowGain=lowGain;
	cdspPvEdgeHighGain=highGain;
	cdspPvEdgeLowThr=lowThr;
	cdspPvEdgeHighThr=highThr;
	cdspPvEdgeYhtbw=yhtbw;
}


void
cdspPvEdgeFilterEnSet(
	UINT8 pvFilterEn
)
{
	cdspPvFilterEn=pvFilterEn;
}

void
cdspIntrPvEdgeFilterEnSet(
	UINT8 pvFilterEn
)
{
	cdspPvFilterEn=pvFilterEn;
}


void 
cdspPvIntLowThrSet(
	UINT8 lowThr
)
{
	cdspPvIntLowThr=lowThr;
}

void 
cdspIntrPvIntLowThrSet(
	UINT8 lowThr
)
{
	cdspPvIntLowThr=lowThr;
}


void 
cdspPvMedianModeSet(
	UINT8 mode
)
{
	cdspPvMedmode=mode;
}

void 
cdspIntrPvMedianModeSet(
	UINT8 mode
)
{
	cdspPvMedmode=mode;
}

void
cdspStillEdgeFilterSet(
	UINT8 ftr0,
	UINT8 ftr1,
	UINT8 ftr2,
	UINT8 ftr3,
	UINT8 ftr4,
	UINT8 ftr5
)
{
	cdspStillEdgeFtr0=ftr0;
	cdspStillEdgeFtr1=ftr1;
	cdspStillEdgeFtr2=ftr2;
	cdspStillEdgeFtr3=ftr3;
	cdspStillEdgeFtr4=ftr4;
	cdspStillEdgeFtr5=ftr5;
}

void
cdspStillEdgeParaSet(
	UINT8 clamp,
	UINT8 div,
	UINT8 lowGain,
	UINT8 highGain,
	UINT8 lowThr,
	UINT8 highThr,
	UINT8 yhtbw
)
{

	cdspStillEdgeClamp=clamp;
	cdspStillEdgeScale=div;
	cdspStillEdgeLowGain=lowGain;
	cdspStillEdgeHighGain=highGain;
	cdspStillEdgeLowThr=lowThr;
	cdspStillEdgeHighThr=highThr;
	cdspStillEdgeYhtbw=yhtbw;
}

void
cdspStillEdgeFilterEnSet(
	UINT8 stillFilterEn
)
{
	cdspStillFilterEn=stillFilterEn;
}

void 
cdspStillIntLowThrSet(
	UINT8 lowThr
)
{
	cdspStillIntLowThr=lowThr;
}


void 
cdspStillYCoringParaSet(
	UINT8 *par
)
{
	UINT8 i;
	for (i=0;i<17;i++){
		cdspStillYCoring[i]=*(par+i);

		cdspPrintf("cor[%bd]=%bd\n",i,cdspStillYCoring[i]);
	}
}

/**
 * @fn        void cdspSharpnessEnhanSet(UINT8 en,	UINT8 yConFactor)
 * @brief     cdspContrastEnhanSet
 * @param     [in] en : enable/disable
 * @param     [in] yConFactor : Y contrast factor (3.5 bit)
 * @retval    NULL
 * @see       NULL
 * @author    LinJieCheng
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void 
cdspSharpnessEnhanSet(
	UINT8 stillDynamicEn,
	UINT8 stillDynamicEnhanMode
)
{
	SysDynamicValue.stillDynamicEn = stillDynamicEn;

	if (SysDynamicValue.stillDynamicEn == CDSP_DYNAMIC_DISABLE) {	
		SysDynamicValue.stillMode.nextEnhanMode = stillDynamicEnhanMode;
		cdspStillDynamicEnhanSet(SysDynamicValue.stillMode.resourceId,SysDynamicValue.stillMode.nextEnhanMode);
		SysDynamicValue.stillMode.currEnhanMode = SysDynamicValue.stillMode.nextEnhanMode;
	}
}

void 
cdspStillDynamicEnhanFlow(
	void
)
{
	UINT8 snapGainIdx;
	UINT8 snapCompensationGainIdx;

	if (SysDynamicValue.stillDynamicEn == CDSP_DYNAMIC_ENABLE) {

		sp1kAeStatusGet(SP1K_AE_G_GAIN,&snapGainIdx);
		sp1kAeStatusGet(SP1K_AE_G_CaptureCompensationGAIN,&snapCompensationGainIdx);

		snapGainIdx += snapCompensationGainIdx;
		
		cdspPrintfDynamic("Snap Gain Idx=%bu ",snapGainIdx);

		if (snapGainIdx > SysDynamicValue.stillMode.weakThr) {			
			cdspPrintfDynamic("Weak\n");			
			SysDynamicValue.stillMode.nextEnhanMode = CDSP_DYNAMIC_WEAK;
		}
		else if (snapGainIdx > SysDynamicValue.stillMode.middleThr) {
			cdspPrintfDynamic("Middle\n");
			SysDynamicValue.stillMode.nextEnhanMode = CDSP_DYNAMIC_MIDDLE;
		}
		else {
			cdspPrintfDynamic("Strong\n");
			SysDynamicValue.stillMode.nextEnhanMode = CDSP_DYNAMIC_STRONG;
		}

		if ( (SysDynamicValue.stillMode.currEnhanMode != SysDynamicValue.stillMode.nextEnhanMode) || 
				SysDynamicValue.stillMode.preResourceId != SysDynamicValue.stillMode.resourceId)	{
			cdspStillDynamicEnhanSet(SysDynamicValue.stillMode.resourceId,SysDynamicValue.stillMode.nextEnhanMode);
			SysDynamicValue.stillMode.currEnhanMode = SysDynamicValue.stillMode.nextEnhanMode;
			SysDynamicValue.stillMode.preResourceId = SysDynamicValue.stillMode.resourceId;
			cdspPrintfDynamic("e\n");
		}	
	}
}

void 
cdspPvDynamicEnhanFlow(
	void
)
{
	UINT8 pvGainIdx;
	UINT16 location;

	if (SysDynamicValue.pvDynamicEn == CDSP_DYNAMIC_ENABLE) {

		aeIntrCurGainValueGet(&pvGainIdx);
		
		cdspPrintfDynamic("GainIdx=%bu\n",pvGainIdx);

		if (pvGainIdx > SysDynamicValue.pvMode.weakThr) {
			cdspPrintfDynamic("W");
			SysDynamicValue.pvMode.nextEnhanMode = CDSP_DYNAMIC_WEAK;
		}
		else if (pvGainIdx > SysDynamicValue.pvMode.middleThr) {
			cdspPrintfDynamic("M");
			SysDynamicValue.pvMode.nextEnhanMode = CDSP_DYNAMIC_MIDDLE;
		}
		else {
			cdspPrintfDynamic("S");
			SysDynamicValue.pvMode.nextEnhanMode = CDSP_DYNAMIC_STRONG;
		}

		location = SysDynamicValue.pvModeOffset + (UINT16)(SysDynamicValue.pvMode.nextEnhanMode * 18);

		//printf("dynamic location=%d\n",location);

		//for (pvGainIdx=0;pvGainIdx>18;pvGainIdx++) {
		//	printf("G_FPGAMemPool[%d]=0x%b02x\n",location+pvGainIdx,G_FPGAMemPool[location+pvGainIdx]);
		//}

		//printf("G_FPGAMemPool[%d]=0x%b02x\n",location+0,G_FPGAMemPool[location+0]);
		//printf("G_FPGAMemPool[%d]=0x%b02x\n",location+2,G_FPGAMemPool[location+2]);
		//printf("G_FPGAMemPool[%d]=0x%b02x\n",location+3,G_FPGAMemPool[location+3]);
		//printf("G_FPGAMemPool[%d]=0x%b02x\n",location+4,G_FPGAMemPool[location+4]);
		//printf("G_FPGAMemPool[%d]=0x%b02x\n",location+5,G_FPGAMemPool[location+5]);
		//printf("G_FPGAMemPool[%d]=0x%b02x\n",location+6,G_FPGAMemPool[location+6]);
		//printf("G_FPGAMemPool[%d]=0x%b02x\n",location+7,G_FPGAMemPool[location+7]);
		//printf("G_FPGAMemPool[%d]=0x%b02x\n",location+8,G_FPGAMemPool[location+8]);
		//printf("G_FPGAMemPool[%d]=0x%b02x\n",location+9,G_FPGAMemPool[location+9]);
		//printf("G_FPGAMemPool[%d]=0x%b02x\n",location+10,G_FPGAMemPool[location+10]);
		//printf("G_FPGAMemPool[%d]=0x%b02x\n",location+11,G_FPGAMemPool[location+11]);
		//printf("G_FPGAMemPool[%d]=0x%b02x\n",location+12,G_FPGAMemPool[location+12]);
		//printf("G_FPGAMemPool[%d]=0x%b02x\n",location+13,G_FPGAMemPool[location+13]);
		//printf("G_FPGAMemPool[%d]=0x%b02x\n",location+14,G_FPGAMemPool[location+14]);
		//printf("G_FPGAMemPool[%d]=0x%b02x\n",location+15,G_FPGAMemPool[location+15]);
		//printf("G_FPGAMemPool[%d]=0x%b02x\n",location+16,G_FPGAMemPool[location+16]);

		cdspIntrPvEdgeParaSet(dynamicIqTbl[location+2],dynamicIqTbl[location+3],dynamicIqTbl[location+4],dynamicIqTbl[location+5],dynamicIqTbl[location+6],dynamicIqTbl[location+7],dynamicIqTbl[location+8]);
		cdspIntrPvEdgeFilterSet(dynamicIqTbl[location+9],dynamicIqTbl[location+14],dynamicIqTbl[location+13],dynamicIqTbl[location+12],dynamicIqTbl[location+11],dynamicIqTbl[location+10]);
		cdspIntrPvEdgeFilterEnSet(dynamicIqTbl[location+0]&0x0f);
		cdspIntrPvIntLowThrSet(dynamicIqTbl[location+16]);
		cdspIntrPvMedianModeSet(dynamicIqTbl[location+15]);
	}

}

void
cdspIntrPvDynamicIqParamSet(
	void
)
{
	XBYTE[ REG_Cdsp_EdgeEn ] &= 0xe0;
	XBYTE[ REG_Cdsp_EdgeEn ] |= (cdspPvFilterEn &0x0F);

	XBYTE[ REG_Cdsp_Yhtclamp ] = cdspPvEdgeClamp;
	XBYTE[ REG_Cdsp_Yhdiv ] = cdspPvEdgeScale & 0x0f;
	XBYTE[ REG_Cdsp_ygain0 ] = cdspPvEdgeLowGain;
	XBYTE[ REG_Cdsp_ygain1 ] = cdspPvEdgeHighGain;
	XBYTE[ REG_Cdsp_yhtlowthr ] = cdspPvEdgeLowThr;
	XBYTE[ REG_Cdsp_yhthighthr ] = cdspPvEdgeHighThr;
	XBYTE[ REG_Cdsp_yhtbw ] = cdspPvEdgeYhtbw;
	XBYTE[REG_Cdsp_intpllowthr]= cdspPvIntLowThr;
	XBYTE[REG_Cdsp_Medmode] = cdspPvMedmode;

	XBYTE[ REG_Cdsp_ftr0 ] = cdspPvEdgeFtr0;
	XBYTE[ REG_Cdsp_ftr5 ] = ( cdspPvEdgeFtr5 & 0x0f );
	XBYTE[ REG_Cdsp_ftr4 ] = ( cdspPvEdgeFtr4 & 0x0f );
	XBYTE[ REG_Cdsp_ftr3 ] = ( cdspPvEdgeFtr3 & 0x0f );
	XBYTE[ REG_Cdsp_ftr2 ] = ( cdspPvEdgeFtr2 );
	XBYTE[ REG_Cdsp_ftr1 ] = ( cdspPvEdgeFtr1 );
}

void
INTR_PvDynamicEnhanCheck(
	void
)
{
	if (SysDynamicValue.pvDynamicEn == CDSP_DYNAMIC_ENABLE) {
		if (SysDynamicValue.pvMode.currEnhanMode != SysDynamicValue.pvMode.nextEnhanMode) {
			cdspIntrPvDynamicIqParamSet();
			SysDynamicValue.pvMode.currEnhanMode = SysDynamicValue.pvMode.nextEnhanMode;
			cdspPrintfDynamic("e\n");
		}
	}
}

/**
 * @fn        void cdspSaturationEnhanSet(	UINT8 en, UINT8 uvsatFactor, UINT8 uOffset, UINT8 vOffset)
 * @brief     cdspSaturationEnhanSet
 * @param     [in] uvsatFactor : UV saturation factor (3.5 bit)
 * @param     [in] UOffset : U Offset ( -128 ~ 127 )
 * @param     [in] VOffset : V Offset ( -128 ~ 127 )
 * @retval    NULL
 * @see       NULL
 * @author    LinJieCheng
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void
cdspSaturationEnhanSet(
	UINT8 en,
	UINT8 uvsatFactor,
	UINT8 uOffset,
	UINT8 vOffset
)
{
	uOffset = uOffset;
	vOffset = vOffset;
	HAL_CdspSatHueEnSet(en);
	HAL_CdspSatHueParaSet(uvsatFactor,
						XBYTE[ REG_Cdsp_huesin1data ],
						XBYTE[ REG_Cdsp_huecosdata ],
						XBYTE[ REG_Cdsp_huesin2data ],
						XBYTE[ REG_Cdsp_UOffset ] ,
						XBYTE[ REG_Cdsp_VOffset ] );
}

/**
 * @fn        void cdspContrastEnhanSet(UINT8 en,	UINT8 yConFactor)
 * @brief     cdspContrastEnhanSet
 * @param     [in] en : enable/disable
 * @param     [in] yConFactor : Y contrast factor (3.5 bit)
 * @retval    NULL
 * @see       NULL
 * @author    LinJieCheng
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void
cdspContrastEnhanSet(
	UINT8 en,
	UINT8 yConFactor
)
{
	HAL_CdspBriContEnSet(en);
	HAL_CdspYBrightOffsetSet(XBYTE[ REG_Cdsp_Yb ]);
	HAL_CdspYContrastOffsetSet(yConFactor);
}

/**
 * @fn        void cdspBrightnessEnhanSet(UINT8 en,	SINT8 brightnessOffset)
 * @brief     cdspBrightnessEnhanSet
 * @param     [in] en : enable/disable
 * @param     [in] brightnessOffset : brightness Offset (2's complement, -128~+127)
 *\n                                               +127 : 01111111
 *\n                                               -128 : 10000000
 * @retval    NULL
 * @see       NULL
 * @author    LinJieCheng
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void
cdspBrightnessEnhanSet(
	UINT8 en,
	SINT8 brightnessOffset
)
{
	HAL_CdspBriContEnSet(en);
	HAL_CdspYBrightOffsetSet(brightnessOffset);
}

/**
 * @fn        void cdspHueEnhanSet(UINT8 en,	SINT8 brightnessOffset)
 * @brief     cdspHueEnhanSet
 * @param     [in] en : enable/disable
 * @param     [in] channel : 0, 1, 2
 * @param     [in] fileId : resource id
 * @retval    NULL
 * @see       NULL
 * @author    LinJieCheng
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void
cdspHueEnhanSet(
	UINT8 en,
	UINT8 channel,
	UINT8 fileId
)
{
	cdspHueBinSet(fileId,channel);
	HAL_CdspHueCorrectionEnSet(en);
}


/****************************************************/
/*   unused function                          
/****************************************************/

#if 0
UINT8 cdspDoYuvHScaling(
	UINT32 srcAddr,
	UINT16 srcXSize,
	UINT16 srcYSize,
	UINT32 dstAddr,
	UINT16 dstXSize,
	UINT16 dstYSize
)
{
	UINT8 reg21d0 = XBYTE[ 0x21d0 ];

	// src
	HAL_Dram1stPathFrmBufDAddrSet( srcAddr );
	HAL_DramDoYuvSrcHSizeSet( srcXSize );

	//dst
	HAL_Dram1stPathFrmBufAAddrSet( dstAddr );
	HAL_CdspFrmCtrlEnSet(0);
	HAL_CdspWriteBufIdxSet( 0 );

	HAL_Dram1stPathFrmBufSizeSet( dstXSize, 2048 );

	HAL_CdspVDramSkipWr( 6, 0 );

	HAL_CdspRawImgSource( 1 );

	HAL_CdspVDramMode( 0, 1 );

	halCdspDoYuvHScaling( srcXSize, srcYSize, dstXSize, dstYSize );

	XBYTE[ 0x21d0 ] = reg21d0;

	return 0;
}

void
cdspDoFinish(
	void
)
{
	UINT16 para1, para2;

	frontDeviceInfoGet( 0, FRONT_DEV_INFO_DATA_FMT, &para1, &para2 );

	if( para1 == FRONT_DATA_FMT_YUV ) { // YUV sensor
		cdspPostProcessReload();
	}

	cdspSenClockSwitch( 0 );

}

void
cdspQvDo(
	UINT8 rawMode,
	UINT32 rawBufAddr,
	UINT32 qvBufAddr,
	UINT16 rawHSize,
	UINT16 rawVSize,
	UINT16 qvHSize,
	UINT16 qvVSize,
	UINT16 hFactor,
	UINT16 vFactor
)
{
	UINT8 reg21d0 = XBYTE[REG_Cdsp_EdgeEn];

	cdspSenClockSwitch( 1 );

	//XBYTE[REG_Cdsp_mirrorline] = 0x00;
	//XBYTE[REG_Cdsp_Skippixel] = 0x00;
	HAL_CdspDramVMirrorSet(0,0,0);	
	HAL_CdspDramVSkippixelSet(0);

	if( rawMode == CDSP_SNAP_YUV) {
		HAL_Dram1stPathFrmBufDAddrSet( rawBufAddr );
		HAL_DramDoYuvSrcHSizeSet( rawHSize );
	}
	else {
	  	HAL_DramRawFrmBufAddrSet( rawBufAddr );

			if( rawMode == CDSP_SNAP_8RAW) {
				HAL_DramRawFrmBufSizeSet( rawHSize / 2, rawVSize );
			}
			else {
				HAL_DramRawFrmBufSizeSet( rawHSize, rawVSize );
			}

		XBYTE[REG_Cdsp_YUVhscaleen] = 0x00;	 /* YUV Scale Down Disable */
	}

	HAL_Dram1stPathFrmBufAAddrSet(qvBufAddr);
	HAL_Dram1stPathFrmBufSizeSet( qvHSize, qvVSize );

	HAL_CdspFrmCtrlEnSet(0);
	HAL_CdspWriteBufIdxSet(0); /* Set Image Buffer */

	//XBYTE[REG_Cdsp_YUVscalesvden]  =  0x00;  /* sync zoom disable */
	HAL_CdspScaleSyncModeSet(0,0); /*immediate update */
	
	HAL_CdspVDramMode(0,0);

	//XBYTE[0x226E]  =  0x88;
	//XBYTE[0x226f]  =  0x01; /* pixel clock between line interval */
	HAL_CdspDramVInterval(0x0188); /* pixel clock between line interval */

	cdspBlockCtrl( rawMode, CDSP_STAGE_QV );

	HAL_CdspImgSnapDo_Scale(rawMode, rawHSize,rawVSize,1,1,qvHSize,qvVSize, hFactor, vFactor);  //small

	XBYTE[0x21d0] = reg21d0;

	cdspSenClockSwitch( 0 );

}

/**
 * @fn        void cdspYCoringCfg(void)
 * @brief     cdspYCoringCfg
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
static void
cdspYCoringCfg(
	void
)
{
	UINT8 yEdgeEn, yCoringEn,lEdgeEn,uvMedianEn;
	UINT8 reg21d0;
	
	//yEdgeEn = XBYTE[ 0x21d0 ] & 0x01;
	//coringOpt = ( XBYTE[ 0x21d0 ] >> 2 ) & 0x01;
	HAL_CdspEdgeCoringMedianEnGet(&yEdgeEn,&lEdgeEn,&yCoringEn,&uvMedianEn);
	HAL_CdspRegGet(REG_Cdsp_EdgeEn,&reg21d0);
	
	if( yEdgeEn == 0x01 && yCoringEn == 0x01 ) {
		cdspYuvCoringOpt[ 0 ] = reg21d0 & ( ~0x01 );
		cdspYuvCoringOpt[ 1 ] = reg21d0 & ( ~0x04 );
		cdspYuvCoringOpt[ 2 ] = 0;

		cdspYuvCoringTime = 1;
	}
	else {
		cdspYuvCoringOpt[ 0 ] = reg21d0;
		cdspYuvCoringOpt[ 1 ] = 0;

		cdspYuvCoringTime = 0;
	}

}

static UINT8
cdspBlockCtrl(
	UINT8 mode,
	UINT8 stage
)
{
	UINT8 postWB;

	postWB = (UINT8)sp1kSnapParamGet( SP1K_SNAP_PARAM_POST_WB_OPT );

	if( stage == CDSP_STAGE_QV ) {
		// QV block control
		cdspPreProcessOff( mode );

		if( postWB == 1 /* && multiLC == 0 */ ) {
			//UINT16 prvRgain, prvBgain, prvGgain;
			//AAA_AWBGainGet(&prvRgain, &prvBgain, &prvGgain); /* will mark// hlc 0319 add */
			//HAL_CdspWBGainSet(prvRgain, prvGgain, prvBgain, prvGgain); /* hlc 0319 add */
			awbGainReset();//wangjg++ for wb gain set
			
			HAL_CdspWBGainEnSet( 1 );

			HAL_CdspGammaEnSet( 1 );
		}

		if( mode == CDSP_SNAP_YUV ) {
			cdspPostProcessOff();
		}
	}

	return CDSP_TASK_SUCCESS;
}

/**
 * @fn        UINT8 cdspYuv2Yuv(UINT32 srcAddr, UINT32 dstAddr, UINT16 hSize, UINT16 vSize, UINT16 doVSize)
 * @brief     do cdsp YUV to YUV
 * @param     [in] srcAddr : YUV source address
 * @param     [in] dstAddr : YUV destination address
 * @param     [in] hSize : YUV source horizontal size
 * @param     [in] vSize : YUV source vertical size
 * @param     [in] doVSize : YUV destination vertical size
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
UINT8
cdspYuv2Yuv(
	UINT32 srcAddr,
	UINT32 dstAddr,
	UINT16 hSize,
	UINT16 vSize,
	UINT16 doVSize
)
{
	HAL_Dram1stPathFrmBufAAddrSet( dstAddr );
	HAL_Dram1stPathFrmBufSizeSet( hSize, vSize );

	HAL_Dram1stPathFrmBufDAddrSet( srcAddr );

	HAL_CdspFrmCtrlEnSet(0);
	HAL_CdspWriteBufIdxSet( 0 );

	HAL_CdspVDramSkipWr( 6, 0 );

	HAL_CdspRawImgSource( 1 );

	HAL_CdspVDramMode( 0, 1 );

	HAL_DramDoYuvSrcHSizeSet( hSize );

	cdspSenClockSwitch( 1 );

	HAL_CdspImgDo( 2, 0, hSize, 0, vSize, 3, 3, 240, doVSize, 0 );  /* small */

	cdspSenClockSwitch( 0 );

	return CDSP_TASK_SUCCESS;
}

/**
 * @fn        UINT8 sp1kCdspManualWbGainSet(UINT8 WbRGain, UINT8 WbBGain)
 * @brief     Manual White balance gain set
 * @param     [in] WbRGain : R channel gain (2.6 format)
 * @param     [in] WbBGain : B channel gain (2.6 format)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     Allen Lin
 * @todo      N/A
 * @bug       N/A
*/
UINT8 sp1kCdspManualWbGainSet(UINT8 WbRGain, UINT8 WbBGain) USING_0
{
	UINT8 RGain,GrGain,BGain,GbGain;

	HAL_CdspWBGainGet(&RGain, &GrGain, &BGain, &GbGain);

//	printf("gr,gb=%bx,%bx\n",GrGain,GbGain);
	WbRGain = (UINT8)((UINT16)(WbRGain*GrGain)/0x40);
	WbBGain = (UINT8)((UINT16)(WbBGain*GbGain)/0x40);

//	printf("r,gr,b,gb=%bx,%bx,%bx,%bx\n",WbRGain, GrGain, WbBGain, GbGain);
	HAL_CdspWBGainSet(WbRGain, GrGain, WbBGain, GbGain);
	HAL_CdspWBGainEnSet( 1 );

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 cdspRawCB(UINT8 mode, UINT32 rawDataAddr, UINT16 rawHSize, UINT16 rawVSize, UINT8 rawMode)
 * @brief     raw data calibration for caputure
 * @param     [in] mode : 
 *							0: bad pixel compensation
 *							1: post white balance 
 * @param     [in] rawDataAddr : raw data address in DRAM
 * @param     [in] rawHSize : raw data horizontal size
 * @param     [in] rawVSize : raw data vertical size
 * @param     [in] rawMode : 
 *								0: 8  bits raw data
 *								1: 16 bits raw data
 *								2: YUV data  
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
UINT8
cdspRawCB(
	UINT8 mode,
	UINT32 rawDataAddr,
	UINT16 rawHSize,
	UINT16 rawVSize,
	UINT8 rawMode
)
{
	UINT8 ret = FAIL;

	if( mode == CDSP_RAW_BAD_PIXEL ) {
		ret=cdspRawBadPixel( rawDataAddr, rawHSize, rawMode);
	}
	else if( mode == SP1K_SNAP_POST_WB ) {
		//printf("do raw WB\n");
		ret = cdspRawProcess( rawDataAddr, rawHSize, rawVSize, rawMode );
	}

	return ret;
}

#endif

