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
 * @file      Hal_cdsp_core.c
 * @brief     Brief
 * @author    will
 * @since     2010-04-08
 * @date      2010-04-08
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "hal.h"
#include "asicreg.h"
#include "initio.h"
#include "dbg_def.h"

#include "sp1k_calibration_api.h"
#include "util_api.h"
#include "front_logi.h"

#include "hal_cdsp.h"
#include "hal_cdsp_core.h" 
#include "hal_cdsp_internal.h"
#include "hal_front.h"
#include "reg_def.h"
#include "hal_dram.h" 

#include "ae_api.h"

#include "sp1k_snap_api.h"  /* it's illegal */
#include "snap_task.h"      /* it's illegal */

#include "hal_cdsp_debug.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define __FILE	__FILE_ID_HAL_CDSP_CORE__

#define CDSP_PARAM_SIZE ( 16 )

#define HAL_CDSP_CORE_TAG_TODO
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define HighByte(x) 	(unsigned char) ((x) >> 8)
#define LowByte(x)  	(unsigned char) (x) 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/



/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static xdata UINT8	G_CdspMaxFactorEnable;

static xdata UINT16	G_CdspMaxVFactor;   /* will: revise */
static xdata UINT16	G_CdspMaxHFactor;   /* will: revise */

static xdata UINT16 yuvScaleHSize;
static xdata UINT16	yuvScaleVSize; 

static xdata UINT16 roiHSize;
static xdata UINT16 roiVSize;

UINT32 xdata snapCdspYuvSrcBufAddr;

UINT8 xdata halCdspBayerPtn;
//static UINT8 halCdspVScaleSubDo( UINT8  mode, UINT16 hoffset, UINT16 dohsize, UINT16 voffset, UINT16 dovsize, UINT8  hmirroren, UINT8	vmirroren, UINT16 hfactor, UINT16 vfactor, UINT16 hfactini, UINT16 vfactini, UINT16 calrealdo, UINT16 wOffsetH, UINT16 wOffsetV, UINT16 VSub );
//static UINT8 halCdspSubDo( UINT8  mode, UINT16 hoffset, UINT16 dohsize, UINT16 voffset, UINT16 dovsize, UINT8	hmirroren, UINT8  vmirroren, UINT16 hfactor, UINT16 vfactor, UINT16 writeHOffset, UINT16 writeVOffset, UINT16 VSub );

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

void 
HAL_CdspSyncModeSet(
	UINT8 mode
)
{
	if( mode >= SYNCZF ) {
		XBYTE[ REG_Cdsp_YUVscalesvden ] = 0x10;
	}
	else {
		XBYTE[ REG_Cdsp_YUVscalesvden ] = ( mode & 0x01 );
	}
}


UINT8
HAL_CdspSyncModeGet(
	void
)
{
	UINT8 val;
	val=XBYTE[REG_Cdsp_YUVscalesvden];

	if ((val&0x10)==0x10){
		return SYNCZF;
	}
	else if((val&0x01)==0x01){
		return SYNCVD;
	}
	else{
		return IMMEDIATE;
	}
}

/**
 * @fn        UINT8 HAL_CdspVScaleYuvSet(UINT8 syncMode, UINT16 srcSize, UINT16 dstSize)
 * @brief     Set CDSP vertical scale down factor in YUV domain (first path)
 * @param     [in] syncMode : v scale sync method (2 bit)
 *							0: immediate update
 *							1: reflected at the next valid VD edge
 *							2: reflected at the zoomfactor update
 * @param     [in] srcSize : vertical source size
 * @param     [in] dstSize : vertical destination size
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspVScaleYuvSet(
	UINT8 syncMode,
	UINT16 srcSize,
	UINT16 dstSize
)
{
	UINT32 factor, hrmder;
	UINT16 f;

	HAL_CdspSyncModeSet(syncMode);

	if( dstSize >= srcSize ) {
		XBYTE[ REG_Cdsp_YUVvscalefactor_low ] = 0;
		XBYTE[ REG_Cdsp_YUVvscalefactor_high ] = 0;
		XBYTE[ REG_Cdsp_YUVvscaleen ] = 0;
	}
	else {
		factor = ( ( ( UINT32 ) dstSize << 16 ) / ( UINT32 ) srcSize );

		hrmder = ( ( UINT32 ) 65536 ) % factor;

		if( hrmder != 0 ) {
			factor++;
		}

		f = (UINT16) factor;

		XBYTE[ REG_Cdsp_YUVvscalefactor_low ] = LowByte( f );
		XBYTE[ REG_Cdsp_YUVvscalefactor_high ] = HighByte( f );

		XBYTE[ REG_Cdsp_Yvscaleaccini_low ] = LowByte( f );
		XBYTE[ REG_Cdsp_Yvscaleaccini_high ] = HighByte( f );
		XBYTE[ REG_Cdsp_YUVvscaleen ] = 0x11;
	}

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspHScaleYuvSet(UINT8 syncMode, UINT16 srcSize, UINT16 dstSize)
 * @brief     Set CDSP horizontal scale down factor in YUV domain (first path)
 * @param     [in] syncMode : h scale sync method (2 bit) 
 *							0: immediate update
 *							1: reflected at the next valid VD edge
 *							2: reflected at the zoomfactor update
 * @param     [in] srcSize : horizontal source size
 * @param     [in] dstSize : horizontal destination size
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspHScaleYuvSet(
	UINT8 syncMode,
	UINT16 srcSize,
	UINT16 dstSize
)
{
	UINT32 factor, hrmder;
	UINT16 f;

	HAL_CdspSyncModeSet(syncMode);

	if( dstSize >= srcSize ) {
		XBYTE[ REG_Cdsp_YUVhscalefactor_low ] = 0;
		XBYTE[ REG_Cdsp_YUVhscalefactor_high ] = 0;
		XBYTE[ REG_Cdsp_YUVhscaleen ] = 0;
	}
	else {
		factor = ( ( ( UINT32 ) dstSize << 16 ) / ( UINT32 ) srcSize );

		hrmder = ( ( UINT32 ) 65536 ) % factor;

		if( hrmder != 0 ) {
			factor++;
		}

		f = (UINT16) factor;

		XBYTE[ REG_Cdsp_YUVhscalefactor_low ] = LowByte( f );
		XBYTE[ REG_Cdsp_YUVhscalefactor_high ] = HighByte( f );

		XBYTE[ REG_Cdsp_Yhscaleaccini_low ] = LowByte( f );
		XBYTE[ REG_Cdsp_Yhscaleaccini_high ] = HighByte( f );
		XBYTE[ REG_Cdsp_YUVhscaleen ] = 0x11;
	}

	return HALK_SUCCESS;
}


/**
 * @fn        UINT8 HAL_Cdsp1stPathWriteOffset(UINT16 HOffset, UINT16 VOffset)
 * @brief     Horizontal/Vertical offset for writing image to DRAM (first path)
 * @param     [in] HOffset : Horizontal offset when writing image to SDRAM (12 Bits)
 * @param     [in] VOffset : Vertical offset when writing image to SDRAM (12 Bits)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_Cdsp1stPathWriteOffset(
	UINT16 HOffset,
	UINT16 VOffset
)
{

	XBYTE[ REG_Cdsp_Whoffset_low ] = LowByte( HOffset );
	XBYTE[ REG_Cdsp_Whoffset_high ] = HighByte( HOffset );

	XBYTE[ REG_Cdsp_Wvoffset_low ] = LowByte( VOffset );
	XBYTE[ REG_Cdsp_Wvoffset_high ] = HighByte( VOffset );

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_Cdsp1stPathYuvSizeSet(UINT16 hsize, UINT16 vsize)
 * @brief     Set the output YUV size in the first path
 * @param     [in] hSize : horizontal YUV size
 * @param     [in] vSize : vertical YUV size
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_Cdsp1stPathYuvSizeSet(
	UINT16 hSize,
	UINT16 vSize
)
{
	yuvScaleHSize = hSize;
	yuvScaleVSize = vSize; 
	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_Cdsp1stPathYuvSizeGet(UINT16 *hSize, UINT16 *vSize)
 * @brief     Get the ouput YUV size in the first path 
 * @param     [in] hSize
 * @param     [in] vSize
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_Cdsp1stPathYuvSizeGet(
	UINT16 *hSize,
	UINT16 *vSize
)
{
	*hSize = yuvScaleHSize;
	*vSize = yuvScaleVSize;
	return HALK_SUCCESS;
}


/**
 * @fn        UINT8 HAL_Cdsp2ndPathCfg(UINT8 skip, UINT8 range, UINT8 sync)
 * @brief     configure 2nd path
 * @param     [in] skip : skip writing image data to dram
 * @param     [in] range : 
 *							0:    0 ~ 256
 *							1: -128 ~ +127
 * @param     [in] sync :
 *							0: immediate update
 *							1: sync with VD								
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
HAL_Cdsp2ndPathCfg(
	UINT8 skip,
	UINT8 range,
	UINT8 sync
)
{

	XBYTE[REG_Cdsp_skipwr1] = (skip&0x01) | (range&0x01)<<1 | (sync&0x01)<<4;

	return SUCCESS; 
}

/**
 * @fn        UINT8 HAL_Cdsp2ndPathEnSet(UINT8 en)
 * @brief     Enable/Disable 2nd path
 * @param     [in] en : 
 *						0: disable
 *						1: enable
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
HAL_Cdsp2ndPathEnSet(
	UINT8 en
)
{
	XBYTE[REG_Cdsp_YUV2ndpathen] = (en&0x01);

	if(en==0){
		HAL_CdspRstBlock(CDSPRstVDram1); /*it needs to reset VDram1 because we turn off VDram1 immediately*/
	}
	return SUCCESS;

}

/**
 * @fn        UINT8 HAL_Cdsp2ndPathWriteOffset(UINT16 HOffset, UINT16 VOffset)
 * @brief     Horizontal/Vertical offset for writing image to DRAM (second path)
 * @param     [in] HOffset : Horizontal offset when writing image to SDRAM (12 Bits)
 * @param     [in] VOffset : Vertical offset when writing image to SDRAM (12 Bits)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_Cdsp2ndPathWriteOffset(
	UINT16 HOffset,
	UINT16 VOffset
)
{

	XBYTE[ REG_Cdsp_whoffset1_low ] = LowByte( HOffset );
	XBYTE[ REG_Cdsp_whoffset1_high ] = HighByte( HOffset );

	XBYTE[ REG_Cdsp_wvoffset1_low ] = LowByte( VOffset );
	XBYTE[ REG_Cdsp_wvoffset1_high ] = HighByte( VOffset );

	return HALK_SUCCESS;
}


/**
 * @fn        UINT8 HAL_Cdsp2ndPathVScaleYuvSet(UINT8 syncMode, UINT16 srcSize, UINT16 dstSize)
 * @brief     Set CDSP vertical scale down factor in YUV domain (second path)
 * @param     [in] syncMode : v scale sync method (2 bit)
 *							0: immediate update
 *							1: reflected at the next valid VD edge
 *							2: reflected at the zoomfactor update
 * @param     [in] srcSize : vertical source size
 * @param     [in] dstSize : vertical destination size
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_Cdsp2ndPathVScaleYuvSet(
	UINT8 syncMode, 
	UINT16 srcSize, 
	UINT16 dstSize
)
{
	UINT32 factor, fv1,fv2;
	//UINT32 hrmder;
	UINT16 f;
	UINT32 fvini,halfSize;

	//if (srcSize<dstSize){
	//	srcSize=dstSize;
	//}	
	
	HAL_CdspSyncModeSet(syncMode);

	if( dstSize >= srcSize ) {
		XBYTE[ REG_Cdsp_YUVvscalefactor2_low ] = 0;
		XBYTE[ REG_Cdsp_YUVvscalefactor2_high ] = 0;
		//XBYTE[ REG_Cdsp_YUVvscaleen2 ] = 0;
	}
	else {
		//factor = ( ( ( UINT32 ) dstSize << 16 ) / ( UINT32 ) srcSize );

		fv1 = ( ( UINT32 ) dstSize << (16+6) );
		//fv2 = ( fv1 / ( UINT32 ) ( frontVSize - 1 ) ) * ( UINT32 ) vfactor;
		fv2 =  (fv1 / ( UINT32 ) ( srcSize- 1 ) );  //will revise
		factor = ( ( fv2 + 63 ) / ( UINT32 ) 64 );			


   		//hrmder = ( ( UINT32 ) 65536 ) % factor;                              
	    //if( hrmder != 0 ) {
		//    factor++;
        //}

		//halfSize = (srcSize+1)>>1;
		halfSize = ( ( UINT32 ) dstSize << 16 ) / ( factor << 1 );
		
		if ( factor < 65536 ) { 
			halfSize++; 
		} 
		

		
		fvini = 65536 - (((halfSize -1)*factor)&0xffff);

		//printf("v ini factor=%lx\n",fvini);


		f = (UINT16) (factor&0xffff) ;
		//printf("v factor=0x%lx\n",factor);
		
		XBYTE[ REG_Cdsp_YUVvscalefactor2_low ] = LowByte( f );
		XBYTE[ REG_Cdsp_YUVvscalefactor2_high ] = HighByte( f );

        f = (UINT16) (fvini&0xffff);
		XBYTE[ REG_Cdsp_YUVvscaleaccini2_low ] = LowByte( f );
		XBYTE[ REG_Cdsp_YUVvscaleaccini2_high ] = HighByte( f );
		//XBYTE[ REG_Cdsp_YUVvscaleaccini2_low ] = 0xff;
		//XBYTE[ REG_Cdsp_YUVvscaleaccini2_high ] = 0xff;
		
		XBYTE[ REG_Cdsp_YUVvscaleen2 ] = 0x11;

   		halCdspPrintf("2nd Path v dstSize=%d,srcSize=%d\n",dstSize,srcSize);
   		halCdspPrintf("2nd Path v factor=0x%lx,ini=0x%lxd\n",factor,fvini);
		
	}


	return HALK_SUCCESS;

}


/**
 * @fn        UINT8 HAL_Cdsp2ndPathHScaleYuvSet(UINT8 syncMode, UINT16 srcSize, UINT16 dstSize)
 * @brief     Set CDSP horizontal scale down factor in YUV domain (second path)
 * @param     [in] syncMode : v scale sync method (2 bit)
 *							0: immediate update
 *							1: reflected at the next valid VD edge
 *							2: reflected at the zoomfactor update
 * @param     [in] srcSize : vertical source size
 * @param     [in] dstSize : vertical destination size
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_Cdsp2ndPathHScaleYuvSet(
	UINT8 syncMode, 
	UINT16 srcSize, 
	UINT16 dstSize
)
{
	UINT32 factor,fh1,fh2;
	//UINT32 hrmder;
	UINT16 f;
	UINT32 fhini,halfSize;


	//if (srcSize<dstSize){
	//	srcSize=dstSize;
	//}
	

	HAL_CdspSyncModeSet(syncMode);

	if( dstSize >= srcSize ) {
		XBYTE[ REG_Cdsp_YUVhscalefactor2_low ] = 0;
		XBYTE[ REG_Cdsp_YUVhscalefactor2_high ] = 0;
		//XBYTE[ REG_Cdsp_YUVhscaleen2 ] = 0;
	}
	else {
		//factor = ( ( ( UINT32 ) dstSize << 16 ) / ( UINT32 ) srcSize );
		fh1 = ( ( UINT32 ) dstSize << (16+6) );
		//fv2 = ( fv1 / ( UINT32 ) ( frontVSize - 1 ) ) * ( UINT32 ) vfactor;
		fh2 =  (fh1 / ( UINT32 ) ( srcSize- 1 ) );  //will revise
		factor = ( ( fh2 + 63 ) / ( UINT32 ) 64 );			


		//hrmder = ( ( UINT32 ) 65536 ) % factor;
		//if( hrmder != 0 ) {
		//	factor++;
		//}


		//fcen = (srcSize+1)>>1;
		halfSize = ( ( UINT32 ) dstSize << 16 ) / ( factor << 1 );
		
		if ( factor < 65536 ) { 
			halfSize++; 
		} 
		
		fhini = 65536 - (((halfSize -1)*factor)&0xffff);


		//printf("h factor=0x%lx\n",factor);
		
		f = (UINT16) (factor&0xffff);
	
		XBYTE[ REG_Cdsp_YUVhscalefactor2_low ] = LowByte( f );
		XBYTE[ REG_Cdsp_YUVhscalefactor2_high ] = HighByte( f );


		f = (UINT16) (fhini &0xffff);
		XBYTE[ REG_Cdsp_YUVhscaleaccini2_low ] = LowByte( f );
		XBYTE[ REG_Cdsp_YUVhscaleaccini2_high ] = HighByte( f );
		//XBYTE[ REG_Cdsp_YUVhscaleaccini2_low ] =  0xff;
		//XBYTE[ REG_Cdsp_YUVhscaleaccini2_high ] = 0xff;
		
		XBYTE[ REG_Cdsp_YUVhscaleen2 ] = 0x11;

    	halCdspPrintf("2nd Path h dstSize=%d,srcSize=%d\n",dstSize,srcSize);
    	halCdspPrintf("2nd Path h factor=0x%lx,hini=0x%lx\n",factor,fhini);
	}

	

	return HALK_SUCCESS;

}

/**
 * @fn        UINT8 HAL_Cdsp2ndPathClampHSize(UINT8 en, UINT16 hsize)
 * @brief     Set horizontal clamp size for sencond path 
 * @param     [in] en : 
 *						0: disable
 *						1: enable
 * @param     [in] hsize : horiztontal clamp size
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_Cdsp2ndPathClampHSize(
	UINT8  en,
	UINT16 hsize
)
{
	XBYTE[ REG_Cdsp_clamphsizeen1    ] = (en & 0x01);
	XBYTE[ REG_Cdsp_clamphsize1_low  ] = (hsize&0xff);
	XBYTE[ REG_Cdsp_clamphsize1_high ] = (hsize>>8);
	
	return HALK_SUCCESS;	
}

/**
 * @fn        UINT8 HAL_Cdsp1stPathZFactorSet(UINT8 updateMode, UINT16 hZFactor, UINT16 vZFactor, UINT16 dstHSize, UINT16 dstVSize, UINT16 roiHSize, UINT16 roiVSize, UINT16 disfactor)
 * @brief     The programmer must know the maximum allowable factor before using this function.
 *			  CDSP gets scale factor for first path output. Source is from RGB horizontal  
 *	  		  scale ouput (it's the same as sensor output usually) and target size is video size
 * @param     [in] updateMode : 
 *					  0: immediate update
 *					  1: reflected at the next valid VD edge
 *					  2: reflected at the zoomfactor update
 * @param     [in] hZFactor : the digital horizontal zoom factor 
 * @param     [in] vFactor : the digital vertical zoom factor
 * @param     [in] dstHSize : destination horizontal size
 * @param     [in] dstVSize : destination vertical size
 * @param     [in] roiHSize : ROI, the horizontal region of source image you wanna get 
 * @param     [in] roiVSize : ROI, the vertical region of source image you wanna get 
 * @param     [in] disFactor : display factor
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_Cdsp1stPathZFactorSet(
	UINT8  updateMode,
	UINT16 hZFactor,
	UINT16 vZFactor,
	UINT16 dstHSize,
	UINT16 dstVSize,
	UINT16 roiHSize,
	UINT16 roiVSize,
	UINT16 aeDisFactor,
	UINT16 awbDisFactor
)
{
	UINT32 x1factreg, x2factreg, yfactreg, vini, h1ini, h2ini, h1factor, h2factor;
	//UINT16 centerx, centery;
	UINT16 hoffset, hsize, voffset, vsize;
	//UINT16 monitorWidth, monitorHeight, tmp;
	UINT16 frontHSize, frontVSize;
	UINT16 zCropHSize,zCropVSize;
	UINT8 crop_henable, crop_venable, scale_en;
	//UINT32 fh1, fh2, fv1, fv2;
	UINT8 yuvSensor;    
    //UINT16 wrHOff, wrVOff;
	UINT16 maxHFac,maxVFac;
	//UINT16 extraCropHSize;
	UINT16 zFactor;
	//printf("HAL_Cdsp1stPathZFactorSet() %d %d %d %d %d %d %d\n", (UINT16)pdateMode, hZFactor, vZFactor, bufWidth, bufHeight, (UINT16)ZUpdateTrigger, disFactor);

	halCdspPrintf("dstWidth=%d,dstHeight=%d\n",dstHSize,dstVSize);
	halCdspPrintf("roiHSize=%d,roiVSize=%d\n",roiHSize,roiVSize);
	halCdspPrintf("z fac = %d\n",hZFactor);
    
	#if 1
		frontPreviewSizeGet(0, &frontHSize, &frontVSize);
		yuvSensor = XBYTE[REG_Front_Ys128en] & 0x10;
		//if(yuvSensor != 0) {    //will : 1628 fix the bug , YUV Size *2  
		//	monitorWidth *= 2;
		//}
  	#else
		frontHSize  = ( XBYTE[ REG_Front_Hsize_high ] << 8 ) | XBYTE[ REG_Front_Hsize_low ];
		frontVSize  = ( XBYTE[ REG_Front_Vsize_high ] << 8 ) | XBYTE[ REG_Front_Vsize_low ];

		YUVSensor = XBYTE[REG_Front_Ys128en] & 0x10;
 	#endif
    
	
	//centerx = ( frontHSize ) / 2;
	//centery = ( frontVSize ) / 2;
	//zCropHSize = ( ( ( UINT32 ) frontHSize ) * 100 ) / hZFactor;
	zCropHSize = (UINT16)(( ( ( UINT32 ) roiHSize ) * 100 ) / (UINT32)hZFactor);

	//printf("zCropHSize=%d\n",zCropHSize);
	#if 0
		wrHOff = 0; 
    	wrVOff = 0;

	   	if (frontHSize != dstHSize){
	       	wrHOff = (frontHSize - dstHSize) >> 1;
	       	//frontWidth = (UINT32)frontWidth * clampWidth / bufWidth;  //will mark
	       	//bufWidth= clampWidth;                                     //will mark 
	   	}

		if (frontVSize != dstVSize){
	       	wrVOff = (frontVSize - dstVSize) >> 1;
	       	//frontHeight = (UINT32)frontHeight * clampHeight / bufHeight;  //will mark
	       	//bufHeight = clampHeight;                                      //will mark
	   	}   
	#endif

	/* [0028661] 20080820 mattwang modify, fix 320x240 video zoom right side noise. */


	HAL_CdspMaxZFactorGet(&maxHFac,&maxVFac);

    halCdspPrintf("max z h fac =%d\n",maxHFac);
	
	scale_en = (hZFactor <= maxHFac) ? 1 : 0;
	scale_en = (G_CdspMaxFactorEnable) ? scale_en : 1;
	//printf("scale_en=%bd\n",scale_en);


	if (((zCropHSize <= CDSP_MAX_LINEBUF) && (scale_en==1) )) {	  /* RGB/YUV input, and HSize <= 1280 */

		/* get crop h size, h offset */
		HAL_CdspScaleFactorGet(roiHSize,dstHSize,&x2factreg,&h2ini,hZFactor);	
		HAL_CdspHScaleSizeGet(roiHSize,dstHSize,x2factreg,h2ini,hZFactor,&hoffset,&hsize,&zCropHSize); /*zCropHSize must be multiple of 8*/
		//printf("roiHSize=%d,dstHSize=%d,hoffset=%d,hsize=%d,zCropHSize=%d\n",roiHSize,dstHSize,hoffset,hsize,zCropHSize);

		/* get crop v size, v offset */
		#if 1
		/* if zCropHSize is changed, h/v factor should be changed as well */
		zFactor= (UINT16)(((UINT32)roiHSize*100)/(UINT32)zCropHSize);	
		HAL_CdspScaleFactorGet(roiVSize,dstVSize,&yfactreg,&vini,zFactor);		
		HAL_CdspVScaleSizeGet(roiVSize,dstVSize,yfactreg ,vini,zFactor,&voffset,&vsize,&zCropVSize);
		#else
		HAL_CdspScaleFactorGet(roiVSize,dstVSize,&yfactreg,&vini,vZFactor);				
		HAL_CdspVScaleSizeGet(roiVSize,dstVSize,yfactreg ,vini,zFactor,&voffset,&vsize,&zCropVSize);				
		#endif
		//printf("roiVSize=%d,dstVSize=%d,voffset=%d,vsize=%d,zCropVSize=%d\n",roiVSize,dstVSize,voffset,vsize,zCropVSize);

		
		if (roiHSize!=frontHSize) {
			hoffset += ((frontHSize-roiHSize)>>1);
		}

		if (roiVSize!=frontVSize) {
			voffset += ((frontVSize-roiVSize)>>1);
		}

		/*  1st path yuv H scale en */
        //if(dstHSize==frontHSize) 
        if ( dstHSize >= roiHSize ) {
			XBYTE[REG_Cdsp_YUVhscalefactor_low] = 0;
			XBYTE[REG_Cdsp_YUVhscalefactor_high] = 0;
			//XBYTE[ REG_Cdsp_YUVhscaleen ] = 0x10;  /* it doesn't sync wait zf, so we don't set 1st path YUV H scale disable here */		   
        }
		else {
		   XBYTE[ REG_Cdsp_YUVhscaleen ] = 0x11;  /* enable 1st path YUV H scale */
		}   

		/*  1st path yuv V scale en */
		//if(dstVSize==frontVSize){			
		if ( dstVSize >= roiVSize ) {			
			XBYTE[ REG_Cdsp_YUVvscalefactor_low] = 0;
			XBYTE[ REG_Cdsp_YUVvscalefactor_high] = 0;
			//XBYTE[ REG_Cdsp_YUVvscaleen ] = 0x10;  /* disable 1st path YUV V scale */		   
		}   
		else {
           XBYTE[ REG_Cdsp_YUVvscaleen ] = 0x11;  /* enable 1st path YUV V scale */
		}
		
		/*  disable RGB H Scale factor =0 */
		XBYTE[ REG_Cdsp_HRGBscalefactor_low ]  = 0;   
		XBYTE[ REG_Cdsp_HRGBscalefactor_high ] = 0;
		XBYTE[ REG_Cdsp_HRGBscaleinieve_low ]  = 0;
		XBYTE[ REG_Cdsp_HRGBscaleinieve_high ] = 0;
		XBYTE[ REG_Cdsp_HRGBscaleiniodd_low ]  = 0;
		XBYTE[ REG_Cdsp_HRGBscaleiniodd_high ] = 0;
		
		/*  1st path yuv H scale factor */
		if ( zCropHSize <= dstHSize ) {    
			XBYTE[ REG_Cdsp_YUVhscalefactor_low ] = 0x00;
			XBYTE[ REG_Cdsp_YUVhscalefactor_high ] = 0x00;
		}
		else {
			XBYTE[ REG_Cdsp_YUVhscalefactor_low ] = READ8( x2factreg, 3 );
			XBYTE[ REG_Cdsp_YUVhscalefactor_high ] = READ8( x2factreg, 2 );
			XBYTE[ REG_Cdsp_Yhscaleaccini_low ] = READ8( h2ini, 3 );  
			XBYTE[ REG_Cdsp_Yhscaleaccini_high ] = READ8( h2ini, 2 );
		}

		/*  1st path yuv V scale factor */
		if ( zCropVSize <= dstVSize ) {
			XBYTE[ REG_Cdsp_YUVvscalefactor_low ] = 0x00;
			XBYTE[ REG_Cdsp_YUVvscalefactor_high ] = 0x00;
		}
		else {
			XBYTE[ REG_Cdsp_Yvscaleaccini_low ] = READ8( vini, 3 );   
			XBYTE[ REG_Cdsp_Yvscaleaccini_high ] = READ8( vini, 2 );   
			XBYTE[ REG_Cdsp_YUVvscalefactor_low ] = READ8( yfactreg, 3 );
			XBYTE[ REG_Cdsp_YUVvscalefactor_high ] = READ8( yfactreg, 2 );
		}

		/* 3A set flow */
		HAL_Cdsp1st3AWinSet(zCropHSize,zCropVSize,aeDisFactor,awbDisFactor);

	}
	else if (yuvSensor==0){   /* will :  RGB input and H Size > 1280, do RGB HScale and YUV Scale) */

		h1factor = 100;
		h2factor = hZFactor;


		HAL_CdspScaleFactorGet(roiHSize,CDSP_MAX_LINEBUF,&x1factreg,&h1ini,h1factor);
		HAL_CdspScaleFactorGet(CDSP_MAX_LINEBUF,dstHSize,&x2factreg,&h2ini,hZFactor);
		HAL_CdspScaleFactorGet(roiVSize,dstVSize,&yfactreg,&vini,vZFactor);			
		
		HAL_CdspHScaleSizeGet(roiHSize,dstHSize,x2factreg,h2ini,hZFactor,&hoffset,&hsize,&zCropHSize);
		HAL_CdspVScaleSizeGet(roiVSize,dstVSize,yfactreg ,vini,vZFactor,&voffset,&vsize,&zCropVSize); 

		
		XBYTE[ REG_Cdsp_HRGBscaleen ] = 0x11;
		XBYTE[ REG_Cdsp_YUVhscaleen ] = 0x11;
		XBYTE[ REG_Cdsp_YUVvscaleen ] = 0x11;

		if( frontHSize == dstHSize ) {//xian 20090915
			XBYTE[ REG_Cdsp_HRGBscalefactor_low ] = 0x00;
			XBYTE[ REG_Cdsp_HRGBscalefactor_high ] = 0x00;
			XBYTE[ REG_Cdsp_YUVhscalefactor_low ] = 0x00;
			XBYTE[ REG_Cdsp_YUVhscalefactor_high ] = 0x00;
		}
		else {
			// 20081218 fix RGB H scale even/odd register
			XBYTE[ REG_Cdsp_HRGBscaleiniodd_low ] = READ8( h1ini, 3 );
			XBYTE[ REG_Cdsp_HRGBscaleiniodd_high ] = READ8( h1ini, 2 );
			if ( x1factreg < 0x8000 ) {
				h1ini = (h1ini >> 1) + 0x8000;
			}
			XBYTE[ REG_Cdsp_HRGBscaleinieve_low ] = READ8( h1ini, 3 );
			XBYTE[ REG_Cdsp_HRGBscaleinieve_high ] = READ8( h1ini, 2 );

			XBYTE[ REG_Cdsp_HRGBscalefactor_low ] = READ8( x1factreg, 3 );    /*will: RGB Horizontal Scale */
			XBYTE[ REG_Cdsp_HRGBscalefactor_high ] = READ8( x1factreg, 2 );
			
			XBYTE[ REG_Cdsp_YUVhscalefactor_low ] = READ8( x2factreg, 3 );   
			XBYTE[ REG_Cdsp_YUVhscalefactor_high ] = READ8( x2factreg, 2 );
			XBYTE[ REG_Cdsp_Yhscaleaccini_low ] = READ8( h2ini, 3 );      
			XBYTE[ REG_Cdsp_Yhscaleaccini_high ] = READ8( h2ini, 2 );
		}

		if( frontVSize == dstVSize ) {//xian 20090915
			XBYTE[ REG_Cdsp_YUVvscalefactor_low ] = 0x00;
			XBYTE[ REG_Cdsp_YUVvscalefactor_high ] = 0x00;
		}
		else {
			if( vsize <= dstVSize ) {
				//vsize = dstVSize;
				//distancey = vsize >> 1;
				//voffset = ( UINT16 ) ( ( UINT32 ) centery - distancey );
				XBYTE[ REG_Cdsp_YUVvscalefactor_low ] = 0x00;
				XBYTE[ REG_Cdsp_YUVvscalefactor_high ] = 0x00;				
			}
			else {
				XBYTE[ REG_Cdsp_Yvscaleaccini_low ] = READ8( vini, 3 );   //will mark
				XBYTE[ REG_Cdsp_Yvscaleaccini_high ] = READ8( vini, 2 );   
				//XBYTE[ 0x21CE ] = READ8( yfactreg, 3 );
				//XBYTE[ 0x21CF ] = READ8( yfactreg, 2 );
				
				XBYTE[ REG_Cdsp_YUVvscalefactor_low ] = READ8( yfactreg, 3 );
				XBYTE[ REG_Cdsp_YUVvscalefactor_high ] = READ8( yfactreg, 2 );
			}
		}

		#ifndef HAL_CDSP_CORE_TAG_TODO
			aeHSize    = ((((UINT32) CDSP_MAX_LINEBUF)*100/disFactor) >> 3)-1;
			aeVSize    = ((((UINT32) vsize)*100/disFactor) >> 3)-1;
			aeHOffset  = (((UINT32) CDSP_MAX_LINEBUF)-(aeHSize << 3)) >> 1;
			aeVOffset  = (((UINT32) vsize)-(aeVSize << 3)) >> 1;

			awbHSize   = ((UINT32) CDSP_MAX_LINEBUF)*100/disFactor * 9 / 10;
			awbVSize   = ((UINT32) vsize)*100/disFactor * 9 / 10;
			awbHOffset = ((UINT32) CDSP_MAX_LINEBUF - awbHSize ) >> 1;
			awbVOffset = ((UINT32) vsize - awbVSize ) >> 1;
		#else
			aeDisFactor = aeDisFactor;
			awbDisFactor = awbDisFactor;
		#endif

	}
	else{
		ASSERT(0,0);
	}

	HAL_CdspSyncModeSet(updateMode);

    #if 1  /*will: revise */
		if( hsize >= frontHSize) {
			crop_henable = 0;
		}
		else {
			crop_henable = 1;
		}
		
		if( vsize >= frontVSize ) {
			crop_venable = 0;
		}
		else {
			crop_venable = 1;
		}
	#else
		if( hsize == frontWidth0 ) {//xian 20090915
			crop_henable = 0;
		}
		else {
			crop_henable = 1;
		}

		if( vsize == frontHeight0 ) {//xian 20090915
			crop_venable = 0;
		}
		else {
			crop_venable = 1;
		}
    #endif

	//printf (" crop_henable=%bd,crop_venable=%bd\n",crop_henable,crop_venable);
	
	//printf (" hsize = %d,  \n", hsize);
	//printf (" vsize = %d,  \n", vsize);
	//printf (" hoffset = %d,  \n", hoffset);
	//printf (" voffset = %d,  \n", voffset);
	//printf (" x2factreg =0x%lx\n",x2factreg);
	//printf (" yfactreg =0x%lx\n",yfactreg);

	//printf("hoffset=%d,voffset=%d\n",hoffset,voffset);
	//printf("zoom crop hsize=%d,zoome crop vsize=%d\n",zCropHSize,zCropVSize);
	

	//hoffset =( hoffset>>1)<<1;     /* offset should be mutiple of 2 */
	//voffset =( voffset>>1)<<1;	   /* offset should be mutiple of 2 */			
	
    if (hsize<dstHSize){       		
		HAL_Cdsp1stPathYuvSizeSet(hsize,vsize);		
		//hsize = ((hsize+7) >>3)<<3;   /* hsize is mutiple of 8 */	
		//HAL_CdspVDramSizeClamp( 1, zCropHSize, 1, zCropVSize, 1 );				
    }
	else{
		HAL_Cdsp1stPathYuvSizeSet(dstHSize,dstVSize);
		//HAL_CdspVDramSizeClamp( 1, dstHSize, 1, dstVSize, 1 );		
    }

	/* set cdsp croping */
	//printf("crop hoffset=%d,crop hsize=%d,crop voffset=%d,crop vsize=%d\n",hoffset,frontHSize-hoffset,voffset,zCropVSize);
	//HAL_CdspCropSet( crop_henable, crop_venable, updateMode, hoffset, hsize, voffset, vsize );
	//HAL_CdspCropSet( crop_henable, crop_venable, updateMode, hoffset, zCropHSize, voffset, zCropVSize );
	HAL_CdspCropSet( crop_henable, crop_venable, updateMode, hoffset, frontHSize-hoffset, voffset, zCropVSize ); /*postpone writing time of awb window to avoid awb error issue*/
	
	//HAL_CdspVDramSizeClamp( 1, hsize, 1, vsize, 1 );

	//HAL_Cdsp1stPathWriteOffset(wrHOff, wrVOff);    //xian ++ 20090915

	/**********************************************************/
	/*  AE/AWB Window ZF update                               */
	/**********************************************************/

	#if 0
		halCdspPrintf(" hZFactor = %d\n", hZFactor);
		halCdspPrintf(" disFactor = %bu\n", disFactor);
		halCdspPrintf(" frontHSize = %d,  \n", frontHSize);
		halCdspPrintf(" frontVSize = %d,  \n", frontVSize);
		halCdspPrintf(" dstHSize = %d,  \n", dstHSize);
		halCdspPrintf(" dstVSize = %d,  \n", dstVSize);
		halCdspPrintf("zoom_crophsize =%d \n",zoom_crophsize); // 761

		halCdspPrintf(" hsize = %d,  \n", hsize);
		halCdspPrintf(" vsize = %d,  \n", vsize);
		halCdspPrintf(" hoffset = %d,  \n", hoffset);
		halCdspPrintf(" voffset = %d,  \n", voffset);
		halCdspPrintf(" centerx = %d,  \n", centerx);
		halCdspPrintf(" centery = %d,  \n", centery);
		halCdspPrintf(" distancex = %ld,	\n", distancex);
		halCdspPrintf(" distancey = %ld,	\n", distancey);
		halCdspPrintf(" fh1 = %ld,  \n", fh1);
		halCdspPrintf(" fh2 = %ld,  \n", fh2);
		halCdspPrintf(" x1factreg = %ld,	\n", x1factreg);
		halCdspPrintf(" x2factreg = %ld,	\n", x2factreg);
		halCdspPrintf(" yfactreg = %ld,  \n", yfactreg);
		halCdspPrintf(" h1ini = %ld,	\n", h1ini);
		halCdspPrintf(" h2ini = %ld,	\n", h2ini);
	
		halCdspPrintf(" vini = %ld,  \n", vini);
		halCdspPrintf(" 0x21c0 = %bx,  \n", XBYTE[0x21c0]);
		halCdspPrintf(" 0x21c8 = %bx,  \n", XBYTE[0x21c8]);
	#endif
	
	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_Cdsp1stPath3AWinSet(UINT16 hSize, UINT16 vSize, UINT16 dispFactor)
 * @brief     HAL_Cdsp1stPath3AWinSet
 * @param     [in] hSize : horizontal size of CDSP output
 * @param     [in] vSize : vertical size of CDSP output
 * @param     [in] dispFactor : display zoom facotr (zoom factor = cdsp zoom factor * display zoom factor)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-7-26
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_Cdsp1st3AWinSet(
	UINT16 hSize,
	UINT16 vSize,
	UINT16 aeDispFactor,
	UINT16 awbDispFactor
)
{
	UINT16 imgHSize,imgVSize;
	UINT16 aeImgHSize,aeImgVSize;
	UINT16 awbHSize,awbVSize;
	UINT16 awbHOffset,awbVOffset;

	//printf("hSize=%d, vSize=%d, aeDispFactor=%d\n",hSize,vSize,aeDispFactor);

	aeImgHSize=hSize;
	aeImgVSize=vSize;

	//printf("aeImgHSize=%d, aeImgVSize=%d\n",aeImgHSize,aeImgVSize);
	
	aeWinConfigSet(aeImgHSize, aeImgVSize, aeDispFactor);

	if (awbDispFactor>100){
		imgHSize=hSize*100/awbDispFactor;
		imgVSize=vSize*100/awbDispFactor;
	}
	else{
		imgHSize=hSize;
		imgVSize=vSize;
	}

	//awbHoffset = (hsize-awbHsize) >> 1;
	//awbVoffset = (vsize-awbVsize) >> 1;
	
	awbHSize   = imgHSize * 9 / 10;	
	awbVSize   = imgVSize * 9 / 10;

	//printf("1 awb VSize=%d\n",awbVSize);
    if ((((UINT32)64<<16)/awbVSize)+1 > 0x8000){
    	
    	awbVSize=(UINT16)(((UINT32)64<<16)/0x7fff);
    	awbVSize = awbVSize+2;
    	//printf("2 awb VSize=%d\n",awbVSize);
    }
	
	awbHOffset = (imgHSize-awbHSize) >> 1;
	awbVOffset = (imgVSize-awbVSize) >> 1;

	awbHOffset = (awbHOffset >> 1)<<1; /* keep h offset to be even */
	awbVOffset = (awbVOffset >> 1)<<1; /* keep v offset to be even */

	#if 0 
		if(hoffset & 0x0001) {
			awbHOffset |= 0x0001;
		}
		else {
			awbHOffset &= 0xFFFE;
		};

		if(voffset & 0x0001) {
			awbVOffset |= 0x0001;
		}
		else {
			awbVOffset &= 0xFFFE;
		};
	#endif


	HAL_CdspAWBWindowSizeSet  ( awbHOffset, awbHSize + awbHOffset -1, awbVOffset, awbVSize+awbVOffset-1, 64 ,64);
	
	return SUCCESS;
}

UINT8
HAL_CdspAWBWinSet(
	UINT16 hSize,
	UINT16 vSize
)
{
	UINT16 awbHSize,awbVSize;
	UINT16 awbHOffset,awbVOffset;

	awbHSize   = hSize * 9 / 10; 
	awbVSize   = vSize * 9 / 10;

	//printf("1 awb VSize=%d\n",awbVSize);
	if ((((UINT32)64<<16)/awbVSize)+1 > 0x8000){
		
		awbVSize=(UINT16)(((UINT32)64<<16)/0x7fff);
		awbVSize = awbVSize+2;
		//printf("2 awb VSize=%d\n",awbVSize);
	}

	awbHOffset = (hSize-awbHSize) >> 1;
	awbVOffset = (vSize-awbVSize) >> 1;

	awbHOffset = (awbHOffset >> 1)<<1; /* keep h offset to be even */
	awbVOffset = (awbVOffset >> 1)<<1; /* keep v offset to be even */

	#if 0 
		if(hoffset & 0x0001) {
			awbHOffset |= 0x0001;
		}
		else {
			awbHOffset &= 0xFFFE;
		};

		if(voffset & 0x0001) {
			awbVOffset |= 0x0001;
		}
		else {
			awbVOffset &= 0xFFFE;
		};
	#endif

	HAL_CdspAWBWindowSizeSet  ( awbHOffset, awbHSize + awbHOffset -1, awbVOffset, awbVSize+awbVOffset-1, 64 ,64);
	return SUCCESS;
}
/**
 * @fn        UINT8 HAL_CdspMaxZFactorGet(UINT16* hFactor, UINT16* vFactor, UINT16 cropHSize, UINT16 cropVSize)
 * @brief     get maximum digital zoom factor of CDSP.  
 * @param     [out] hFactor : the maximum horiztontal zoom factor that CDSP can work 
 * @param     [out] vFactor : the maximum vertical zoom factor that CDSP can work
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspMaxZFactorGet(
	UINT16 *hZFactor,
	UINT16 *vZFactor
)
{
	
	//UINT16 srcHSize,srcVSize;
	//srcHSize = (UINT16)XBYTE[REG_Front_Hsize_low]+(UINT16)XBYTE[REG_Front_Hsize_high]*256;
	//srcVSize = (UINT16)XBYTE[REG_Front_Vsize_low]+(UINT16)XBYTE[REG_Front_Vsize_high]*256;	
	//*hZFactor = ( UINT16 ) (  (( ( UINT32 )srcHSize - 1 ) *100) / ( (UINT32)320 - 1 ) );    /*will revise*/
	//*vZFactor = ( UINT16 ) (  (( ( UINT32 )srcVSize - 1 ) *100) / ( (UINT32)240 - 1 ) );  /*will revise*/
	UINT16 dispVSize;
	
	halCdspPrintf("roi h=%d,v=%d\n",roiHSize,roiVSize);

	dispVSize=(UINT16)((((UINT32)roiVSize*320/((UINT32)roiHSize))>>1)<<1);	
	halCdspPrintf(" disp v=%d\n", dispVSize);
	
	//*hZFactor = ( UINT16 ) (  (( ( UINT32 )roiHSize - 1 ) *100) / ( (UINT32)320 - 1 ) );    /*will revise*/
	//*vZFactor = ( UINT16 ) (  (( ( UINT32 )roiVSize - 1 ) *100) / ( (UINT32)secPathVsize - 1 ) );  /*will revise*/
	*hZFactor = ( UINT16 ) (  (( ( UINT32 )roiHSize  ) *100) /  320  );    /*will revise*/
	*vZFactor = ( UINT16 ) (  (( ( UINT32 )roiVSize  ) *100) /  dispVSize  );  /*will revise*/
	
	halCdspPrintf(" hf=%d vf=%d\n", *hZFactor, *vZFactor);
	*hZFactor = ( ( *hZFactor ) < 100 ) ? 100 : ( *hZFactor );
	*vZFactor = ( ( *vZFactor ) < 100 ) ? 100 : ( *vZFactor );
	//printf(" hf=%d vf=%d\n", *vfactor, *hfactor);

	if (G_CdspMaxFactorEnable == 0x01){
		*hZFactor = ( ( *hZFactor ) < G_CdspMaxHFactor ) ? ( *hZFactor ) : G_CdspMaxHFactor;
		*vZFactor = ( ( *vZFactor ) < G_CdspMaxVFactor ) ? ( *vZFactor ) : G_CdspMaxVFactor;
	}

	//printf(" hf=%d vf=%d\n", *vfactor, *hfactor);

	return SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspMaxZFactorSet(UINT16 hZFactor, UINT16 vZFactor, UINT8 enable)
 * @brief     Set the maximum zoom factor of CDSP
 * @param     [in] hZFactor : maximum horizontal zoom factor
 * @param     [in] vZFactor : maximum vertical zoom factor
 * @param     [in] enable : 
 *							0: disable the maximum zoom factor
 *							1: enable the maximum zoom factor
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspMaxZFactorSet(
	UINT16 hZFactor,
	UINT16 vZFactor,
	UINT8 enable
)
{
//	printf("HAL_CdspMaxZFactorSet() %d %d %d\n", hfactor, vfactor, (UINT16)enable);

	G_CdspMaxHFactor = hZFactor;
	G_CdspMaxVFactor = vZFactor;
	G_CdspMaxFactorEnable =  enable;

	return SUCCESS;
}

/**
 * @fn        void HAL_CdspScaleFactorGet(UINT16 srcSize, UINT16 dstSize, UINT32 *scaleFactor, UINT32 *iniFactor, UINT16 zFactor)
 * @brief     Get sacle factor and initial factor for scale engine
 * @param     [in] srcSize : source size
 * @param     [in] dstSize : destination size
 * @param     [out] scaleFactor : scale factor
 * @param     [out] iniFactor : initial factor
 * @param     [in] zFactor : digital zoom factor
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_CdspScaleFactorGet(
	UINT16 srcSize,
	UINT16 dstSize,
	UINT32 *scaleFactor,
	UINT32 *iniFactor,
	UINT16 zFactor
)
{
	UINT32 fact;
	UINT32 ini;
	UINT32 halfSize;
		 
	fact = ( ( UINT32 ) dstSize << (16+4) );
	fact =  (fact / ( UINT32 ) ( srcSize- 1 ) ) * ( UINT32 ) zFactor;  
	fact = ( ( fact + 1599 ) / ( UINT32 ) 1600 );			

	halfSize = ( ( UINT32 ) dstSize << 16 ) / ( fact << 1 );

	if ( fact < 65536 ) { 
		halfSize++; 
	} 
	
	ini = 65536 - ( ( ( halfSize - 1 ) * fact ) & 0xFFFF );

	*scaleFactor = fact;//(fact&0xffff);
	*iniFactor = ini;//(ini&0xffff);

	halCdspPrintf("fact=%ld,ini=%ld\n",fact,ini);
}


/**
 * @fn        void HAL_CdspHScaleSizeGet(UINT16 srcSize, UINT16 dstSize, UINT32 scaleFactor, UINT32 iniFactor, UINT16 zoomFactor, UINT16 *offset, UINT16 *outSize, UINT16 *cropSize)
 * @brief     Get horizontal output size and crop size while applying scale factor and initial factor
 * @param     [in] srcSize : source size
 * @param     [in] dstSize : destination size
 * @param     [in] scaleFactor : scale factor
 * @param     [in] iniFactor : initial factor
 * @param     [in] zoomFactor : digital zoom factor
 * @param     [out] offset : offset for crop size
 * @param     [out] outSize : scale engine output size
 * @param     [out] cropSize : crop size
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_CdspHScaleSizeGet(
	UINT16 srcSize,
	UINT16 dstSize,
	UINT32 scaleFactor,
	UINT32 iniFactor,
	UINT16 zoomFactor,
	UINT16 *offset,
	UINT16 *outSize,
	UINT16 *cropSize
)
{
	UINT32 halfSize;

	if ( scaleFactor < 65536 ) { 
		halfSize = ( ( UINT32 ) dstSize << 16 ) / ( scaleFactor << 1 );
		halfSize++; 				
		*cropSize = halfSize<<1;	
	} 
	else{
		halfSize=((UINT32)srcSize*100/(UINT32)(zoomFactor*2));
		*cropSize = (((halfSize<<1)+7)>>3)<<3;
		halfSize = *cropSize >>1;
	}

	//printf("srcSize=%d\n",srcSize); 
	//printf("halfSize=%ld\n",halfSize);
	
	*offset = ((srcSize/2)-(UINT16)halfSize);			
	*offset = (*offset>>1)<<1;		
	
	//printf("offset=%d,cropSize=%d\n",*offset,*cropSize);

	if (scaleFactor <65536){
		*outSize = (UINT16)((iniFactor+((UINT32)*cropSize-1)*scaleFactor)>>16);
	}
	else{		
		*outSize = *cropSize;
		//*outSize = halfSize<<1;
	}	

	//printf("outSize=%d\n",*outSize);
}

/**
 * @fn        void HAL_CdspVScaleSizeGet(UINT16 srcSize, UINT16 dstSize, UINT32 scaleFactor, UINT32 iniFactor, UINT16 zoomFactor, UINT16 *offset, UINT16 *outSize, UINT16 *cropSize)
 * @brief     Get vertical output size and crop size while applying scale factor and initial factor
 * @param     [in] srcSize : source size
 * @param     [in] dstSize : destination size
 * @param     [in] scaleFactor : scale factor
 * @param     [in] iniFactor : initial factor
 * @param     [in] zoomFactor : digital zoom factor
 * @param     [out] offset : offset for crop size
 * @param     [out] outSize : scale engine output size
 * @param     [out] cropSize : crop size
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_CdspVScaleSizeGet(
	UINT16 srcSize,
	UINT16 dstSize,
	UINT32 scaleFactor,
	UINT32 iniFactor,
	UINT16 zoomFactor,
	UINT16 *offset,
	UINT16 *outSize,
	UINT16 *cropSize
)
{
	UINT32 halfSize;

	if ( scaleFactor < 65536 ) { 
		halfSize = ( ( UINT32 ) dstSize << 16 ) / ( scaleFactor << 1 );
		halfSize++; 				
		*cropSize = halfSize<<1;	
	} 
	else{
		halfSize=((UINT32)srcSize*100/(UINT32)(zoomFactor<<1));
		*cropSize = halfSize<<1;
	}

	//printf("srcSize=%d\n",srcSize); 
	//printf("halfSize=%ld\n",halfSize);
	
	*offset = ((srcSize/2)-(UINT16)halfSize);		
	*offset = (*offset>>1)<<1;		
	
	//printf("offset=%d,cropSize=%d\n",*offset,*cropSize);

	if (scaleFactor <65536){
		*outSize = (UINT16)((iniFactor+((UINT32)*cropSize-1)*scaleFactor)>>16);
	}
	else{		
		//*outSize = *cropSize;
		*outSize = halfSize<<1;
	}	

}


void
HAL_CdspRoiSizeSet(
	UINT16 hSize,
	UINT16 vSize
)
{
	roiHSize=hSize;
	roiVSize=vSize;
}

/****************************************************/
/*  Backup                                          */
/****************************************************/
#if 0

static UINT8
halCdspSubDo(
	UINT8  mode,
	UINT16 hoffset,
	UINT16 dohsize,
	UINT16 voffset,
	UINT16 dovsize,
	UINT8  hmirroren,
	UINT8  vmirroren,
	UINT16 hfactor,
	UINT16 vfactor,
	UINT16 writeHOffset,
	UINT16 writeVOffset,
	UINT16 VSub
)
{
	UINT8 pixel = (UINT8)( voffset % 2 );

	UINT32 vrmder;

	if( vfactor != 0 ) {
		vrmder = ( ( UINT32 ) 65536 ) % ( ( UINT32 ) vfactor );
	}
	else {
		vrmder = 0;
	}

	if( cdspYuvCoringTime == 0 ) {
		if( mode == 2 ) {
			// YUV flow. set D buffer. YUV src size
			HAL_Dram1stPathFrmBufDAddrSet( snapCdspYuvSrcBufAddr );
			HAL_DramDoYuvSrcHSizeSet( snapRawHSize );
		}

		HAL_CdspImgDoSub( mode, hoffset, dohsize, voffset, dovsize, hmirroren, vmirroren, hfactor, vfactor, writeHOffset, writeVOffset, 0, pixel, vrmder );
	}
	else if( cdspYuvCoringTime == 1 ){
		ASSERT( ( ( dovsize + VSub ) <= snapYuvToYuvVSize ), 0 );

		if( mode == 2 ) {
			// YUV flow. set D buffer. YUV src size
			HAL_Dram1stPathFrmBufDAddrSet( snapCdspYuvSrcBufAddr );
			HAL_DramDoYuvSrcHSizeSet( snapRawHSize );
		}

		HAL_Dram1stPathFrmBufCAddrSet( snapCdspYuvToYuvBufAddr0 );
		HAL_CdspFrmCtrlEnSet(0);
		HAL_CdspWriteBufIdxSet( 2 );

		cdspYuvCoringSet( 0 );

#if 0   // do not set frame buffer size. It has to be same as the original
		HAL_SetFBSize( CDSP_MAX_YUV_H_SIZE, dovsize + VSub );
#endif

		XBYTE[REG_Dram_TargetYuv]=0x00;		//{7'b0, targetyuv}

		HAL_CdspImgDoSub( mode, hoffset, dohsize, voffset, dovsize, hmirroren, vmirroren, hfactor, vfactor, 0, 0, 0, pixel, vrmder );

		// buffer set
		HAL_Dram1stPathFrmBufDAddrSet( snapCdspYuvToYuvBufAddr0 );
		HAL_CdspFrmCtrlEnSet(0);
		HAL_CdspWriteBufIdxSet( snapBufNum );

		cdspYuvCoringSet( 1 );

		if( mode != 2 ) {
			cdspPostprocessSwitch( 0 );
		}

		// do not set frame buffer size. It has to be same as the original
		HAL_DramDoYuvSrcHSizeSet( snapRawHSize );

		XBYTE[REG_Dram_TargetYuv]=0x01;		//{7'b0, targetyuv}

		HAL_CdspImgDoSub( 2, 0, dohsize, 0, dovsize, hmirroren, vmirroren, hfactor, vfactor, writeHOffset, writeVOffset, 1, pixel, vrmder );

		// reload parameter
		cdspYuvCoringSet( 0 );

		if( mode != 2 ) {
			cdspPostprocessSwitch( 1 );
		}
	}

	return 0;
}

UINT8
HAL_CdspImgDoABBuf(
	UINT8 mode,
	UINT16 hoffset,
	UINT16 dohsize,
	UINT16 voffset,
	UINT16 dovsize,
	UINT8 hmirroren,
	UINT8 vmirroren,
	UINT16 writeVOffset
)
{
	UINT16 i;
	UINT16 hSize;
	UINT16 vSub, hSub;

	// start from left boundary, hoffset is 0
	hoffset = 0;

	XBYTE[ 0x2276 ] = 0x00;
	XBYTE[ 0x226E ] = 0x88;
	XBYTE[ 0x226f ] = 0x00;

	halCdspSubSizeCal( mode, 0, &hSize, &hSub, &vSub );

	if( dohsize <= hSize ) {
		halCdspSubDo( mode, 0, dohsize, voffset, dovsize, hmirroren, vmirroren, 0, 0, 0, writeVOffset, vSub );
	}
	else {
		halCdspSubDo( mode, 0, hSize, voffset, dovsize, 1, vmirroren, 0, 0, 0, writeVOffset, vSub );
		for( i = hSize - hSub; i < ( dohsize - hSize - hSub ); i = i + hSize ) {
			halCdspSubDo( mode, i, hSize, voffset, dovsize, 0, vmirroren, 0, 0, i + hSub, writeVOffset, vSub );
		}
		halCdspSubDo( mode, i, dohsize - i - hSub, voffset, dovsize, 2, vmirroren, 0, 0, i + hSub, writeVOffset, vSub );
	}

	return 0;
}

UINT8
HAL_CdspImgVDoC_Scale(
	UINT8 mode,
	UINT16 hoffset,
	UINT16 dohsize,
	UINT16 voffset,
	UINT16 dovsize,
	UINT8 hmirroren,
	UINT8 vmirroren,
	UINT16 dosubwidth,
	UINT16 calrealdo, //YUV lines
	UINT16 tghsize,
	UINT16 tgvsize
)
{
	UINT16 i, j;
	UINT16 VSub, HSub, Hsize;
	UINT32 hrmder, vrmder, tmpvini;
	UINT32 factor1;
	UINT16 hfactor, vfactor, hfactini, vfactini;
	UINT16 predosubheight;
	UINT16 dstPosY = 0;
	UINT16 vrealdo; // pre-scaling size
	UINT16 dosubheight; // pre-scaling size each time
	UINT16 lastV;

	dosubwidth = dosubwidth;

	XBYTE[ 0x2276 ] = 0x00;
	XBYTE[ 0x226E ] = 0x88;
	XBYTE[ 0x226f ] = 0x00;
	XBYTE[ REG_Cdsp_EdgeEn ] = XBYTE[ REG_Cdsp_EdgeEn ] | 0x20;

	hrmder = ( ( UINT32 ) 65536 ) % ( ( ( UINT32 ) tghsize << 16 ) / ( UINT32 ) dohsize );
	vrmder = ( ( UINT32 ) 65536 ) % ( ( ( UINT32 ) tgvsize << 16 ) / ( UINT32 ) dovsize );

	if( tghsize >= dohsize ) {
		hfactor = 0;
	}
	else {
		factor1 = ( ( ( UINT32 ) tghsize << 16 ) / ( UINT32 ) dohsize );
		if( hrmder != 0 ) {
			factor1++;
		}
		hfactor = ( UINT16 ) factor1;
		hfactini = hfactor;
	}
	XBYTE[ 0x21c0 ] = 0x00;   // disable YUV Hscale

	if( tgvsize >= dovsize ) {
		vfactor = 0;
	}
	else {
		factor1 = ( ( ( UINT32 ) tgvsize << 16 ) / ( UINT32 ) dovsize );
		if ( vrmder != 0 ) {
			factor1++;
		}
		vfactor = ( UINT16 ) factor1;
		vfactini = vfactor;
	}
	tmpvini = 0;

	VSub = cdspYuvCoringTime << 1;
	Hsize = 256 - ( cdspYuvCoringTime << 4 );
	HSub = 8 + ( cdspYuvCoringTime << 3);
	if ( vrmder == 0 ) {
		if ( mode <= 1 ) {
			VSub += 4;
			Hsize -= 16;
		}
		else {
			VSub += 2;
		}
	}
	else {
		if ( mode <= 1 ) {
			VSub += 5;
			Hsize -= 16;
		}
		else {
			VSub += 3;
		}
	}

	HAL_CarryWork( &tmpvini, &vrealdo, calrealdo, vfactor );
	dosubheight = vrealdo;
	predosubheight = vrealdo;

	if( dohsize <= Hsize && dovsize <= dosubheight ) {
		halCdspVScaleSubDo( mode, hoffset, dohsize, voffset, dovsize, hmirroren, vmirroren, hfactor, vfactor, hfactini, vfactini, tgvsize, 0, 0, VSub );

		dstPosY += calrealdo;
		snapDateStampCdspCallback( dstPosY, calrealdo );
	}
	else {
		if( dohsize <= Hsize ) {
			halCdspVScaleSubDo( mode, hoffset, dohsize, voffset, dovsize, hmirroren, vmirroren, hfactor, vfactor, hfactini, vfactini, tgvsize, 0, 0, VSub );

			dstPosY += calrealdo;
			snapDateStampCdspCallback( dstPosY, calrealdo );
		}
		else {
			if( dovsize <= dosubheight ) {
				halCdspVScaleSubDo( mode, 0, Hsize, voffset, dovsize, 1, vmirroren, hfactor, vfactor, hfactini, vfactini, tgvsize, 0, 0, VSub );

				for( i = Hsize - HSub; i < ( dohsize - Hsize - HSub ); i = i + Hsize ) {
					halCdspVScaleSubDo( mode, i, Hsize, voffset, dovsize, 0, vmirroren, hfactor, vfactor, hfactini, vfactini, tgvsize, i + HSub, 0, VSub );
				}
				halCdspVScaleSubDo( mode, i, dohsize - i - HSub, voffset, dovsize, 2, vmirroren, hfactor, vfactor, hfactini, vfactini, tgvsize, i + HSub, 0, VSub );

				dstPosY += calrealdo;
				snapDateStampCdspCallback( dstPosY, calrealdo );
			}
			else {
				halCdspVScaleSubDo( mode, 0, Hsize, 0, dosubheight, 1, 1, hfactor, vfactor, hfactini, vfactini, calrealdo, 0, 0, VSub );

				for( i = Hsize - HSub; i < ( dohsize - Hsize - HSub ); i = i + Hsize ) {
     				halCdspVScaleSubDo( mode, i, Hsize, 0, dosubheight, 0, 1, hfactor, vfactor, hfactini, vfactini, calrealdo, i + HSub, 0, VSub );  // (1,2)
				}

				halCdspVScaleSubDo( mode, i, dohsize - i - HSub, 0, dosubheight, 2, 1, hfactor, vfactor, hfactini, vfactini, calrealdo, i + HSub, 0, VSub );  // (1,2)

				dstPosY += calrealdo;
				snapDateStampCdspCallback( dstPosY, calrealdo );

				if( vrmder == 0 ) {
				}
				else {
					vfactini = ( UINT16 ) tmpvini;
				}
				HAL_CarryWork( &tmpvini, &vrealdo, calrealdo, vfactor );
				dosubheight = vrealdo;

				//						return; // for testing

				for( j = predosubheight - VSub; j < ( dovsize - dosubheight - VSub ); j = j + predosubheight ) {
					halCdspVScaleSubDo( mode, 0, Hsize, j, dosubheight, 1, 0, hfactor, vfactor, hfactini, vfactini, calrealdo, 0, j + VSub, VSub );
					for( i = Hsize - HSub; i < ( dohsize - Hsize - HSub ); i = i + Hsize ) {
						halCdspVScaleSubDo( mode, i, Hsize, j, dosubheight, 0, 0, hfactor, vfactor, hfactini, vfactini, calrealdo, i + HSub, j + VSub, VSub );
					}
					halCdspVScaleSubDo( mode, i, dohsize - i - HSub, j, dosubheight, 2, 0, hfactor, vfactor, hfactini, vfactini, calrealdo, i + HSub, j + VSub, VSub );

					dstPosY += calrealdo;
					snapDateStampCdspCallback( dstPosY, calrealdo );

					if( vrmder == 0 ) {
					}
					else {
						vfactini = ( UINT16 ) tmpvini;
					}

					predosubheight = dosubheight;

					HAL_CarryWork( &tmpvini, &vrealdo, calrealdo, vfactor );
					dosubheight = vrealdo;
				}

				if( ( dovsize - j - VSub ) == 0 ) {
				}
				else {
					lastV = tgvsize % calrealdo;

					if( lastV == 0 ) {
						lastV = calrealdo;
					}

					halCdspVScaleSubDo( mode, 0, Hsize, j, dovsize - j - VSub, 1, 2, hfactor, vfactor, hfactini, vfactini, lastV, 0, j + VSub, VSub );

					for( i = Hsize - HSub; i < ( dohsize - Hsize - HSub ); i = i + Hsize ) {
						halCdspVScaleSubDo( mode, i, Hsize, j, dovsize - j - VSub, 0, 2, hfactor, vfactor, hfactini, vfactini, lastV, i + HSub, j + VSub, VSub );
					}

					halCdspVScaleSubDo( mode, i, dohsize - i - HSub, j, dovsize - j - VSub, 2, 2, hfactor, vfactor, hfactini, vfactini, lastV, i + HSub, j + VSub, VSub );

					snapDateStampCdspCallback( tgvsize, tgvsize - dstPosY );
				}
			}
		}
	}

	return 0;
}

UINT8
snapDateStampCdspCallback(
	UINT16 yPos,
	UINT16 lines
)
{
	UINT32 addr;
	UINT16 pos, number;
	UINT8 photoFrmOpt;
	UINT8 dateStampType = (UINT8)(snapParam[ SP1K_SNAP_PARAM_DATE_STAMP_TYPE ]);

	photoFrmOpt = (UINT8)sp1kSnapParamGet(SP1K_SNAP_PARAM_DATE_STAMP_TYPE);
	
	if( photoFrmOpt == 1 ) {
		pos = ( yPos + ( snapYuvLines - 1 ) ) / snapYuvLines * snapYuvLines;

		if( pos == yPos ) {
			number = pos / snapYuvLines - 1;

			addr = snapYuv1Addr + (UINT32)snapYuvHSize * ( yPos - snapYuvLines );

			snapPhotoFrmPaste( addr, number );
		}
		else {
			number = pos / snapYuvLines - 1;

			addr = snapYuv1Addr + (UINT32)snapYuvHSize * ( pos - snapYuvLines );

			snapPhotoFrmPaste( addr, number );
		}

//		printf( "yPos %d, pos %d, num %d\n", yPos, pos, number );
	}

	snapDateStampDo( snapYuv1Addr + (UINT32)( yPos - lines ) * snapYuvHSize, dateStampType, snapYuvABBufOpt, snapYuvLines, yPos );

	return 0;
}

UINT8
HAL_CdspVScaleDoABBuf(
	UINT8 mode,
	UINT16 hoffset,
	UINT16 dohsize,
	UINT16 voffset,
	UINT16 dovsize,
	UINT8 hmirroren,
	UINT8 vmirroren,
	UINT16 hfactor,
	UINT16 vfactor,
	UINT16 hfactini,
	UINT16 vfactini,
	UINT16 calrealdo,
	UINT16 writeVOffset
)
{
	UINT16 i;
	UINT16 hSize;
	UINT16 vSub, hSub;

	// start from left boundary, hoffset is 0
	hoffset = 0;

	XBYTE[ 0x2276 ] = 0x00;
	XBYTE[ 0x226E ] = 0x88;
	XBYTE[ 0x226f ] = 0x00;

	halCdspSubSizeCal( mode, 0, &hSize, &hSub, &vSub );

	if( dohsize <= hSize ) {
		halCdspVScaleSubDo( mode, 0, dohsize, voffset, dovsize, hmirroren, vmirroren, hfactor, vfactor, hfactini, vfactini, calrealdo, 0, writeVOffset, vSub );
	}
	else {
		halCdspVScaleSubDo( mode, 0, hSize, voffset, dovsize, 1, vmirroren, hfactor, vfactor, hfactini, vfactini, calrealdo, 0, writeVOffset, vSub );

		for( i = hSize - hSub; i < ( dohsize - hSize - hSub ); i = i + hSize ) {
			halCdspVScaleSubDo( mode, i, hSize, voffset, dovsize, 0, vmirroren, hfactor, vfactor, hfactini, vfactini, calrealdo, i + hSub, writeVOffset, vSub );
		}

		halCdspVScaleSubDo( mode, i, dohsize - i - hSub, voffset, dovsize, 2, vmirroren, hfactor, vfactor, hfactini, vfactini, calrealdo, i + hSub, writeVOffset, vSub );
	}

	return 0;
}

void
halCdspSubSizeCal(
	UINT8 mode,
	UINT32 vrmder,
	UINT16 *hSize,
	UINT16 *hSub,
	UINT16 *vSub
)
{
	*vSub = cdspYuvCoringTime << 1;
	*hSize = 256 - ( cdspYuvCoringTime << 4 );
	*hSub = 8 + ( cdspYuvCoringTime << 3);

	if( ( XBYTE[0x21D0] & 0x0f) != 0 ) {
		XBYTE[0x21D0]  = (XBYTE[0x21D0] | 0x10) ;
		if ( vrmder == 0 ) {
			if ( mode <= 1 ) {
				*vSub += 4;
			}
			else {
				*vSub += 2;
			}
		}
		else {
			if ( mode <= 1 ) {
				*vSub += 5;
			}
			else {
				*vSub += 3;
			}
		}

		*hSize -= 16;
	}
	else {
		XBYTE[0x21D0]  =0;
		if ( vrmder == 0 ) {
			if ( mode <= 1 ) {
				*vSub = 2;
				*hSub = 8;
				*hSize = 256 - 16;
			}
			else {
				*vSub = 0;
				*hSub = 0;
				*hSize = 256;
			}
		}
		else {
			if ( mode <= 1 ) {
				*vSub = 3;
				*hSub = 8;
				*hSize = 256 - 16;
			}
			else {
				*vSub = 0;
				*hSub = 0;
				*hSize = 256;
			}
		}
	}

	return;
}

static UINT8
halCdspVScaleSubDo(
	UINT8  mode,
	UINT16 hoffset,
	UINT16 dohsize,
	UINT16 voffset,
	UINT16 dovsize,
	UINT8  hmirroren,
	UINT8  vmirroren,
	UINT16 hfactor,
	UINT16 vfactor,
	UINT16 hfactini,
	UINT16 vfactini,
	UINT16 calrealdo,
	UINT16 wOffsetH,
	UINT16 wOffsetV,
	UINT16 VSub
)
{
	UINT8 pixel = (UINT8)( voffset % 2 );
	UINT32 vrmder;

	XBYTE[ REG_Cdsp_EdgeEn ] = XBYTE[ REG_Cdsp_EdgeEn ] | 0x20;

	if( vfactor != 0 ) {
		vrmder = ( ( UINT32 ) 65536 ) % ( ( UINT32 ) vfactor );
	}
	else {
		vrmder = 0;
	}

	if( cdspYuvCoringTime == 0 ) {
		if( mode == 2 ) {
			// YUV flow. set D buffer. YUV src size
			HAL_Dram1stPathFrmBufDAddrSet( snapCdspYuvSrcBufAddr );
			HAL_DramDoYuvSrcHSizeSet( snapRawHSize );
		}

		HAL_CdspDoSub_YUVVertical_Scale( mode, hoffset, dohsize, voffset, dovsize, hmirroren, vmirroren, hfactor, vfactor, hfactini, vfactini, calrealdo, wOffsetH, wOffsetV, pixel ); // (1,1)
	}
	else if( cdspYuvCoringTime == 1 ){
		ASSERT( ( ( dovsize + VSub ) <= snapYuvToYuvVSize ), 0 );

		if( mode == 2 ) {
			// YUV flow. set D buffer. YUV src size
			HAL_Dram1stPathFrmBufDAddrSet( snapCdspYuvSrcBufAddr );
			HAL_DramDoYuvSrcHSizeSet( snapRawHSize );
		}

		// first pass
		HAL_Dram1stPathFrmBufCAddrSet( snapCdspYuvToYuvBufAddr0 );
		HAL_CdspFrmCtrlEnSet(0);
		HAL_CdspWriteBufIdxSet( 2 );

		cdspYuvCoringSet( 0 );

#if 0   // do not set frame buffer size. It has to be same as the original
		HAL_SetFBSize( CDSP_MAX_YUV_H_SIZE, dohsize + VSub );
#endif

		XBYTE[REG_Dram_TargetYuv]=0x00;		//{7'b0, targetyuv}

		HAL_CdspImgDoSub( mode, hoffset, dohsize, voffset, dovsize, hmirroren, vmirroren, 0, 0, 0, 0, 0, pixel, vrmder );

		// second pass

		// buffer set
		HAL_Dram1stPathFrmBufDAddrSet( snapCdspYuvToYuvBufAddr0 );
		HAL_CdspFrmCtrlEnSet(0);
		HAL_CdspWriteBufIdxSet( snapBufNum );

		cdspYuvCoringSet( 1 );

		if( mode != 2 ) {
			cdspPostprocessSwitch( 0 );
		}

		// do not set frame buffer size. It has to be same as the original
		HAL_DramDoYuvSrcHSizeSet( snapRawHSize );

		XBYTE[REG_Dram_TargetYuv]=0x01;		//{7'b0, targetyuv}

		HAL_CdspDoSub_YUVVertical_Scale( 2, 0, dohsize, 0, dovsize, hmirroren, vmirroren, hfactor, vfactor, hfactini, vfactini, calrealdo, wOffsetH, wOffsetV, pixel ); // (1,1)

		// reload parameter
		cdspYuvCoringSet( 0 );

		if( mode != 2 ) {
			cdspPostprocessSwitch( 1 );
		}
	}

	return 0;
}

void
cdspYuvCoringSet(
	UINT8 n
)
{
	UINT16 i;

	UINT16 para1, para2;

	frontDeviceInfoGet( 0, FRONT_DEV_INFO_DATA_FMT, &para1, &para2 );

	XBYTE[ 0x21d0 ] = ( XBYTE[ 0x21d0 ] & (~0x0f) ) | ( cdspYuvCoringOpt[ n ] & (~0xf0) );

	if( para1 != FRONT_DATA_FMT_YUV ) { // YUV sensor
		if( cdspYuvCoringTime >= 1 ) {
			for( i = 0; i < CDSP_PARAM_SIZE; i++ ) {
				if( n == 0 ) {
					XBYTE[ 0x2170 + i ] = cdspParam1[ i ];
				}
				else if( n == 1 ) {
					XBYTE[ 0x2170 + i ] = cdspParam2[ i ];
				}
			}
		}
	}

	return;
}

void
cdspPostprocessSwitch(
	UINT8 opt
)
{
	if( opt == 0 ) {
#if 0
		cdspHC = XBYTE[ 0x2170 ];
		XBYTE[ 0x2170 ] = 0x00;
#endif

		cdspSatHue = XBYTE[ 0x21B0 ];
		XBYTE[ 0x21B0 ] = 0x00;
	}
	else {
#if 0
		XBYTE[ 0x2170 ] = cdspHC;
#endif
		XBYTE[ 0x21B0 ] = cdspSatHue;
	}

	return;
}

UINT8
HAL_CdspDoSub_YUVVertical_Scale(
	UINT8 mode,
	UINT16 hoffset,
	UINT16 dohsize,
	UINT16 voffset,
	UINT16 dovsize,
	UINT8 hmirroren,
	UINT8 vmirroren,
	UINT16 hfactor,
	UINT16 vfactor,
	UINT16 hfactini,
	UINT16 vfactini,
	UINT16 calrealdo,
	UINT16 wOffsetH,
	UINT16 wOffsetV,
	UINT8 pixel
)
{
	UINT32 hrmder, vrmder, tmp0, tmp1;
	UINT8 yuvCoringBlock;

	if( ( XBYTE[ 0x21d0 ] & 0x0f ) != 0x00 ) {
		yuvCoringBlock = 1;
	}
	else {
		yuvCoringBlock = 0;
	}

	hfactini = 0;

	if( vfactor != 0 ) {
		vrmder = ( ( UINT32 ) 65536 ) % ( ( UINT32 ) vfactor );
	}
	else {
		vrmder = 0;
	}

	// FIXME: assume H scale is done by RGB domain.
	// For YUV->YUV H scale should be done in YUV domain
	// Currently, H scale is not support
	XBYTE[ REG_Cdsp_YUVhscaleen ] = 0; // Disable YUV H scale

	if( hfactor == 0 ) {
		XBYTE[ REG_Cdsp_HRGBscaleen ] = 0;		  /* Disable RGB Hscale */
		hrmder = 0;
		//  	printf("hrmder = 0x%lx \n",hrmder);
	}
	else {
		ASSERT( 0, 0 );
		// FIXME: does not maintain H scale
		hrmder = 1;

		XBYTE[ REG_Cdsp_HRGBscaleen ] = 0x11;

		XBYTE[ REG_Cdsp_HRGBscalefactor_low ] = LowByte( hfactor );
		XBYTE[ REG_Cdsp_HRGBscalefactor_high ] = HighByte( hfactor );
		XBYTE[ REG_Cdsp_HRGBscaleinieve_low ] = LowByte( ( hfactor / 2 ) + 0x8000 );
		XBYTE[ REG_Cdsp_HRGBscaleinieve_high ] = HighByte( ( hfactor / 2 ) + 0x8000 );
		XBYTE[ REG_Cdsp_HRGBscaleiniodd_low ] = LowByte( hfactor );
		XBYTE[ REG_Cdsp_HRGBscaleiniodd_high ] = HighByte( hfactor );
		//  	  printf("hfactor = 0x%x \n",hfactor);
	}

	if( vfactor == 0 ) {
		XBYTE[ REG_Cdsp_YUVvscaleen ] = 0;		   /* Disable vscale */
		//	 printf("vrmder = 0x%lx \n",vrmder);
	}
	else {
		XBYTE[ REG_Cdsp_YUVvscaleen ] = 0x11;

		XBYTE[ REG_Cdsp_YUVvscalefactor_low ] = LowByte( vfactor );
		XBYTE[ REG_Cdsp_YUVvscalefactor_high ] = HighByte( vfactor );
		XBYTE[ REG_Cdsp_Yvscaleaccini_low ] = LowByte( vfactini );
		XBYTE[ REG_Cdsp_Yvscaleaccini_high ] = HighByte( vfactini );
		//  	printf("vfactor = 0x%x \n",vfactor);
	}
	//	printf("pixsel = 0x%bx\n",pixsel);

	tmp0 = ( UINT32 ) ( dohsize );
	if( yuvCoringBlock != 0 ) {
		if( hmirroren == 0 ) {
			if ( hrmder == 0 ) {
				if ( mode <= 1 ) {
					XBYTE[ REG_Cdsp_Skippixel ] = 0x04;  //Source RGB
				}
				else {
					XBYTE[ REG_Cdsp_Skippixel ] = 0x06;  //Source YUV
				}
			}
			else {
				XBYTE[ REG_Cdsp_EdgeEn ] = 0x00;
				XBYTE[ REG_Cdsp_Skippixel ] = 0x05;  //Source
				XBYTE[ REG_Cdsp_pixsel ] = ( halCdspBayerPtn & 0xFE ) | ( halCdspBayerPtn ^ 0x01 );
			}

			XBYTE[ REG_Cdsp_EsmirEn ] &= 0xF0;
			XBYTE[ REG_Cdsp_intplmirror ] &= 0xF0;

			tmp0 += 16;
		}
		else if( hmirroren == 1 ) {
			XBYTE[ REG_Cdsp_EsmirEn ] &= 0xF0;
			if ( hrmder == 0 ) {
				XBYTE[ REG_Cdsp_EsmirEn ] |= 0x01;
			}
			else {
				XBYTE[ REG_Cdsp_EdgeEn ] = 0x00;
			}

			XBYTE[ REG_Cdsp_Skippixel ] = 0x00;
			XBYTE[ REG_Cdsp_intplmirror ] = ( XBYTE[ REG_Cdsp_intplmirror ] & 0xF0 ) | 0x01;

			tmp0 += 8;
		}
		else if( hmirroren == 2 ) {
			XBYTE[ REG_Cdsp_EsmirEn ] &= 0xF0;
			if ( hrmder == 0 ) {
				if ( mode <= 1 ) {
					XBYTE[ REG_Cdsp_Skippixel ] = 0x04;  //Source RGB
				}
				else {
					XBYTE[ REG_Cdsp_Skippixel ] = 0x06;  //Source YUV
				}
				XBYTE[ REG_Cdsp_EsmirEn ] |= 0x02;
			}
			else {
				XBYTE[ REG_Cdsp_EdgeEn ] = 0x00;
				XBYTE[ REG_Cdsp_Skippixel ] = 0x05;  //Source RGB
				XBYTE[ REG_Cdsp_pixsel ] = ( halCdspBayerPtn & 0xFE ) | ( halCdspBayerPtn ^ 0x01 );
			}

			XBYTE[ REG_Cdsp_intplmirror ] = ( XBYTE[ REG_Cdsp_intplmirror ] & 0xF0 ) | 0x02;

			tmp0 += 8;
		}
		else if( hmirroren == 3 ) {
			XBYTE[ REG_Cdsp_EsmirEn ] &= 0xF0;
			if ( hrmder == 0 ) {
				XBYTE[ REG_Cdsp_EsmirEn ] |= 0x03;
			}
			else {
				XBYTE[ REG_Cdsp_EdgeEn ] = 0x00;
			}

			XBYTE[ REG_Cdsp_Skippixel ] = 0x00;
			XBYTE[ REG_Cdsp_intplmirror ] = ( XBYTE[ REG_Cdsp_intplmirror ] & 0xF0 ) | 0x03;
		}
	}
	else { // yuvCoringBlock == 0
		if( hmirroren == 0 ) {
			if ( hrmder == 0 ) {
				if ( mode <= 1 ) {
					XBYTE[ REG_Cdsp_Skippixel ] = 0x06;  //Source RGB
				}
				else {
					XBYTE[ REG_Cdsp_Skippixel ] = 0x00;  //Source YUV
				}
			}
			else {
				XBYTE[ REG_Cdsp_EdgeEn ] = 0x00;
				XBYTE[ REG_Cdsp_Skippixel ] = 0x05;  //Source
				XBYTE[ REG_Cdsp_pixsel ] = ( halCdspBayerPtn & 0xFE ) | ( halCdspBayerPtn ^ 0x01 );
			}

			XBYTE[ REG_Cdsp_EsmirEn ] &= 0xF0;
			XBYTE[ REG_Cdsp_intplmirror ] &= 0xF0;

			tmp0 += 16;
		}
		else if( hmirroren == 1 ) {
			XBYTE[ REG_Cdsp_EsmirEn ] &= 0xF0;

			if ( hrmder == 0 ) {
				XBYTE[ REG_Cdsp_EsmirEn ] |= 0x01;
			}
			else {
				XBYTE[ REG_Cdsp_EdgeEn ] = 0x00;
			}

			XBYTE[ REG_Cdsp_Skippixel ] = 0x00;
			XBYTE[ REG_Cdsp_intplmirror ] = ( XBYTE[ REG_Cdsp_intplmirror ] & 0xF0 ) | 0x01;

			tmp0 += 8;
		}
		else if( hmirroren == 2 ) {
			XBYTE[ REG_Cdsp_EsmirEn ] &= 0xF0;
			if ( hrmder == 0 ) {
				if ( mode <= 1 ) {
					XBYTE[ REG_Cdsp_Skippixel ] = 0x06;  //Source RGB
				}
				else {
					XBYTE[ REG_Cdsp_Skippixel ] = 0x00;  //Source YUV
				}

				XBYTE[ REG_Cdsp_EsmirEn ] |= 0x02;
			}
			else {
				XBYTE[ REG_Cdsp_EdgeEn ] = 0x00;
				XBYTE[ REG_Cdsp_Skippixel ] = 0x05;  //Source RGB
				XBYTE[ REG_Cdsp_pixsel ] = ( halCdspBayerPtn & 0xFE ) | ( halCdspBayerPtn ^ 0x01 );
			}

			XBYTE[ REG_Cdsp_intplmirror ] = ( XBYTE[ REG_Cdsp_intplmirror ] & 0xF0 ) | 0x02;

			tmp0 += 8;
		}
		else if( hmirroren == 3 ) {
			XBYTE[ REG_Cdsp_EsmirEn ] &= 0xF0;

			if ( hrmder == 0 ) {
				XBYTE[ REG_Cdsp_EsmirEn ] |= 0x03;
			}
			else {
				XBYTE[ REG_Cdsp_EdgeEn ] = 0x00;
			}

			XBYTE[ REG_Cdsp_Skippixel ] = 0x00;
			XBYTE[ REG_Cdsp_intplmirror ] = ( XBYTE[ REG_Cdsp_intplmirror ] & 0xF0 ) | 0x03;
		}
	}

	XBYTE[ REG_Cdsp_Rhoffset_low ]  = READ8( hoffset, 1 );
	XBYTE[ REG_Cdsp_Rhoffset_high ] = READ8( hoffset, 0 );

	XBYTE[ REG_Cdsp_Rhsize_low ]  = READ8( tmp0, 3 );
	XBYTE[ REG_Cdsp_Rhsize_high ] = READ8( tmp0, 2 );

	XBYTE[ REG_Cdsp_Whoffset_low ]  = READ8( wOffsetH, 1 );
	XBYTE[ REG_Cdsp_Whoffset_high ] = READ8( wOffsetH, 0 );

	tmp0 = ( UINT32 ) ( dovsize );

	if( yuvCoringBlock != 0 ) {
	 	if( vmirroren == 0 ) {
			XBYTE[ REG_Cdsp_mirrorline ] = 0x00;  //Source RGB or YUV
			if ( vrmder == 0 ) {
				if ( mode <= 1 ) {
	 				tmp0 += 8;
	 			}
				else {
	 				tmp0 += 4;
	 			}
			}
			else {
				if ( mode <= 1 ) {
	 				tmp0 += 9;

					if ( pixel == 0 ) {
						XBYTE[ REG_Cdsp_pixsel ] = halCdspBayerPtn;
					}
					else {
						XBYTE[ REG_Cdsp_pixsel ] = ( halCdspBayerPtn & 0xFD ) | ( halCdspBayerPtn ^ 0x02 );
					}
	 			}
				else {
	 				tmp0 += 5;
	 			}
			}
		}
		else if( vmirroren == 1 ) {
			if ( mode <= 1 ) {
				XBYTE[ REG_Cdsp_mirrorline ] = 0x24;  //Source RGB
				tmp0 += 4;

				XBYTE[ REG_Cdsp_pixsel ] = halCdspBayerPtn;
			}
			else {
				XBYTE[ REG_Cdsp_mirrorline ] = 0x22;  //Source YUV
				tmp0 += 2;
			}
		}
		else if( vmirroren == 2 ) {
			if ( vrmder == 0 ) {
				if ( mode <= 1 ) {
					XBYTE[ REG_Cdsp_mirrorline ] = 0x14;  //Source RGB
					tmp0 += 4;
				}
				else {
					XBYTE[ REG_Cdsp_mirrorline ] = 0x12;  //Source YUV
					tmp0 += 2;
				}
			}
			else {
				if ( mode <= 1 ) {
					XBYTE[ REG_Cdsp_mirrorline ] = 0x14;  //Source RGB
					tmp0 += 5;

					if ( pixel == 0 ) {
						XBYTE[ REG_Cdsp_pixsel ] = halCdspBayerPtn;
					}
					else {
						XBYTE[ REG_Cdsp_pixsel ] = ( halCdspBayerPtn & 0xFD ) | ( halCdspBayerPtn ^ 0x02 );
					}
				}
				else {
					XBYTE[ REG_Cdsp_mirrorline ] = 0x12;  //Source YUV
					tmp0 += 3;
				}
			}
		}
		else if( ( vmirroren == 3 ) ) {
			if ( mode <= 1 ) {
				XBYTE[ REG_Cdsp_mirrorline ] = 0x34;  //Source RGB

				XBYTE[ REG_Cdsp_pixsel ] = halCdspBayerPtn;
			}
			else {
				XBYTE[ REG_Cdsp_mirrorline ] = 0x32;  //Source YUV
			}
		}
	}
	else { // yuvCoringBlock == 0
	 	if( vmirroren == 0 ) {
			XBYTE[ REG_Cdsp_mirrorline ] = 0x00;  //Source RGB or YUV
			if ( vrmder == 0 ) {
				if ( mode <= 1 ) {
	 				tmp0 += 4;
	 			}
				else {
	 				tmp0 += 0;
	 			}
			}
			else {
				if ( mode <= 1 ) {
	 				tmp0 += 5;

					if ( pixel == 0 ) {
						XBYTE[ REG_Cdsp_pixsel ] = halCdspBayerPtn;
					}
					else {
						XBYTE[ REG_Cdsp_pixsel ] = ( halCdspBayerPtn & 0xFD ) | ( halCdspBayerPtn ^ 0x02 );
					}
	 			}
				else {
	 				tmp0 += 1;
	 			}
			}
		}
		else if( vmirroren == 1 ) {
			if( mode <= 1 ) {
				XBYTE[ REG_Cdsp_mirrorline ] = 0x22;  //Source YUV
				tmp0 += 2;

				XBYTE[ REG_Cdsp_pixsel ] = halCdspBayerPtn;
			}
			else {
				XBYTE[ REG_Cdsp_mirrorline ] = 0x00;
			}
		}
		else if( vmirroren == 2 ) {
			if ( vrmder == 0 ) {
				if ( mode <= 1 ) {
					XBYTE[ REG_Cdsp_mirrorline ] = 0x12;  //Source RGB
					tmp0 += 2;
				}
				else {
					XBYTE[ REG_Cdsp_mirrorline ] = 0x00;  //Source YUV
					tmp0 += 0;
				}
			}
			else {
				if ( mode <= 1 ) {
					XBYTE[ REG_Cdsp_mirrorline ] = 0x12;  //Source RGB
					tmp0 += 3;

					if ( pixel == 0 ) {
						XBYTE[ REG_Cdsp_pixsel ] = halCdspBayerPtn;
					}
					else {
						XBYTE[ REG_Cdsp_pixsel ] = ( halCdspBayerPtn & 0xFD ) | ( halCdspBayerPtn ^ 0x02 );
					}
				}
				else {
					XBYTE[ REG_Cdsp_mirrorline ] = 0x00;  //Source YUV
					tmp0 += 1;
				}
			}
		}
		else if( ( vmirroren == 3 ) ) {
			if ( mode <= 1 ) {
				XBYTE[ REG_Cdsp_mirrorline ] = 0x32;  //Source RGB

				XBYTE[ REG_Cdsp_pixsel ] = halCdspBayerPtn;
			}
			else {
				XBYTE[ REG_Cdsp_mirrorline ] = 0x00;  //Source YUV
			}
		}
	}

	XBYTE[ REG_Cdsp_Rvsize_low ]  = READ8( tmp0, 3 );
	XBYTE[ REG_Cdsp_Rvsize_high ] = READ8( tmp0, 2 );

#if 0
	if( vfactor == 0 ) {
		XBYTE[ REG_Cdsp_Wvoffset_low ]  = READ8( wOffsetV, 1 );
		XBYTE[ REG_Cdsp_Wvoffset_high ] = READ8( wOffsetV, 0 );
	}
	else {
		if ( vrmder != 0 ) {
			wOffsetV++;
		}
		tmp0 = ( ( ( UINT32 ) wOffsetV * ( UINT32 ) vfactor ) >> 16 ) & ~7;

		XBYTE[ REG_Cdsp_Wvoffset_low ]  = READ8( tmp0, 3 );
		XBYTE[ REG_Cdsp_Wvoffset_high ] = READ8( tmp0, 2 );
	}
#else
	XBYTE[ REG_Cdsp_Wvoffset_low ]  = READ8( wOffsetV, 1 );
	XBYTE[ REG_Cdsp_Wvoffset_high ] = READ8( wOffsetV, 0 );
#endif

	XBYTE[ REG_Cdsp_Rvoffset_low ]  = READ8( voffset, 1 );
	XBYTE[ REG_Cdsp_Rvoffset_high ] = READ8( voffset, 0 );

	if( hfactor == 0 ) {
		tmp0 = dohsize;
	}
	else {
		tmp0 = ( ( ( UINT32 ) dohsize * ( UINT32 ) hfactor ) >> 16 ) & ~7;
	}

	if( vfactor == 0 ) {
		tmp1 = dovsize;
	}
	else {
#if 0
		tmp1 = ( ( ( UINT32 ) dovsize * ( UINT32 ) vfactor ) >> 16 ) + 1;
		if( tmp1 > ( UINT32 ) calrealdo ) {
			tmp1 = ( UINT32 ) calrealdo;
		}
		else {
			tmp1 = ( UINT32 ) ( finalvsize );
			do {
				tmp1 = tmp1 - ( UINT32 ) calrealdo;
			} while( tmp1 > ( UINT32 ) calrealdo );
		}
#else
		tmp1 = calrealdo;
#endif
	}

	HAL_CdspVDramSizeClamp( 1, ( UINT16 ) tmp0, 0, ( UINT16 ) tmp1, 0 );  /* Clamp the size to SDRAM */
	//  	   printf(" tmp0 = %ld, tmp1 = %ld.\n",tmp0,tmp1);

	XBYTE[ 0x2121 ] &= ~0x10;  // lens disable

	//  printf (" 0x2262 = %bx \n", XBYTE[0x2262]);
	XBYTE[ 0x2262 ] = 0xff;
	//  printf (" 0x2262 = %bx \n", XBYTE[0x2262]);

	XBYTE[ REG_Cdsp_evnt1 ] = 0xff;

	HAL_CdspRegBackup(snapCdspRegAddr,snapCdspRegSize,&snapCdspRegCnt);

	XBYTE[ REG_Cdsp_Remode ] = 0x10 | ( mode & 0x03 );   /* Set DoCDSP = 1 */
	do {
		tmp0 = XBYTE[ REG_Cdsp_Remode ];  	  /* Wait DoCDSP */
		//		printf("begin do \n");
	} while( ( tmp0 & 0x10 ) == 0x10 );
	//			printf("Call DoSub over \n");
	//  				   printf("21f0 = %bx \n",XBYTE[0x21f0]);
	//			   printf("2002 = %bx \n",XBYTE[0x2002]);
	/*  printf("before DO DO \n");
	  for (i=0;i<0x200 ;i++)
	  {
		printf("XBYTE[0x%x]=0x%bx \n",REG_Cdsp_macroSel+i,XBYTE[REG_Cdsp_macroSel+i]);
	  }
	  for (i=0;i<0x100 ;i++)
	  {
		printf("XBYTE[0x%x]=0x%bx \n",0x2700+i,XBYTE[0x2700+i]);
	  }
	  printf (" 0x21ee = %bx \n", XBYTE[REG_Cdsp_evnt1]);*/


	//printf ("0x2260 = %bx \n", XBYTE[0x2260]);

	/* printf (" 0x21ee = %bx \n", XBYTE[REG_Cdsp_evnt1]);
	 printf (" XBYTE[REG_Cdsp_Remode] = %bx \n", XBYTE[REG_Cdsp_Remode]);
	 printf (" 0x2130 = %bx \n", XBYTE[0x2130]);
	*/

#ifdef CMING
	printf( " Front Input Size \n" );
	printf( " 0x22F0 = %bx \n", XBYTE[ 0x22F0 ] );
	printf( " 0x22F1 = %bx \n", XBYTE[ 0x22F1 ] );
	printf( " 0x22F2 = %bx \n", XBYTE[ 0x22F2 ] );
	printf( " 0x22F3 = %bx \n", XBYTE[ 0x22F3 ] );

	printf( " RGB Hscale Output Size \n" );
	printf( " 0x22F4 = %bx \n", XBYTE[ 0x22F4 ] );
	printf( " 0x22F5 = %bx \n", XBYTE[ 0x22F5 ] );
	printf( " 0x22F6 = %bx \n", XBYTE[ 0x22F6 ] );
	printf( " 0x22F7 = %bx \n", XBYTE[ 0x22F7 ] );

	printf( " YUV Mux Output Size \n" );
	printf( " 0x22F8 = %bx \n", XBYTE[ 0x22F8 ] );
	printf( " 0x22F9 = %bx \n", XBYTE[ 0x22F9 ] );
	printf( " 0x22FA = %bx \n", XBYTE[ 0x22FA ] );
	printf( " 0x22FB = %bx \n", XBYTE[ 0x22FB ] );

	printf( " YUV Scale Output Size \n" );
	printf( " 0x22FC = %bx \n", XBYTE[ 0x22FC ] );
	printf( " 0x22FD = %bx \n", XBYTE[ 0x22FD ] );
	printf( " 0x22FE = %bx \n", XBYTE[ 0x22FE ] );
	printf( " 0x22FF = %bx \n", XBYTE[ 0x22FF ] );
#endif

	return 0;
}

/**
 * @fn        UINT8 HAL_CdspImgSnapDo_Scale(UINT8 mode, UINT16 dohsize, UINT16 dovsize, UINT8 hmirroren, UINT8 vmirroren, UINT16 tghsize, UINT16 tgvsize, UINT16 hFactor, UINT16 vFactor)
 * @brief     HAL_CdspImgSnapDo_Scale
 * @param     [in] mode
 * @param     [in] dohsize
 * @param     [in] dovsize
 * @param     [in] hmirroren
 * @param     [in] vmirroren
 * @param     [in] tghsize
 * @param     [in] tgvsize
 * @param     [in] hFactor
 * @param     [in] vFactor
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspImgSnapDo_Scale(
	UINT8 mode,
	UINT16 dohsize,
	UINT16 dovsize,
	UINT8 hmirroren,
	UINT8 vmirroren,
	UINT16 tghsize,
	UINT16 tgvsize,
	UINT16 hFactor,
	UINT16 vFactor
)
{
	UINT32 m, n, factor1, hrmder;
	UINT16 hsf, vsf, skip;
	UINT32 roiXSize, roiYSize, xOffset, yOffset;
	UINT32 roiXSize8, xOffset8;

	XBYTE[ 0x226E ] = 0x88;
	XBYTE[ 0x226f ] = 0x00;

	roiXSize = (UINT32)dohsize * (UINT32)100 / (UINT32)hFactor;
	xOffset = ( dohsize - roiXSize ) >> 1;
	xOffset &= ~1;

	if( tghsize >= roiXSize ) {
		hsf = 0;
	}
	else {
		factor1 = ( ( ( UINT32 ) tghsize << 16 ) / ( UINT32 ) roiXSize ) + 1;
		hsf = ( UINT16 ) factor1;
	}

	if( mode != 2 ) {
		XBYTE[ 0x21D0 ] = XBYTE[ 0x21D0 ] & 0x02;
	}
	else {
		XBYTE[ 0x21D0 ] = 0x00;
	}

	if( mode != 2 ) {
		if( hsf == 0 ) {
			XBYTE[ REG_Cdsp_HRGBscaleen ] = 0;  		/* Disable RGB Hscale */
			hrmder = 0;
		}
		else {
			XBYTE[ REG_Cdsp_HRGBscaleen ] = 0x11;
			hrmder = ( ( UINT32 ) 65536 ) % ( ( UINT32 ) hsf );

			XBYTE[ REG_Cdsp_HRGBscalefactor_low ]  = READ8( hsf, 1 );
			XBYTE[ REG_Cdsp_HRGBscalefactor_high ] = READ8( hsf, 0 );
			XBYTE[ REG_Cdsp_HRGBscaleinieve_low ]  = LowByte( ( hsf >> 1 ) + 0x8000 );
			XBYTE[ REG_Cdsp_HRGBscaleinieve_high ] = HighByte( ( hsf >> 1 ) + 0x8000 );
			XBYTE[ REG_Cdsp_HRGBscaleiniodd_low ]  = READ8( hsf, 1 );
			XBYTE[ REG_Cdsp_HRGBscaleiniodd_high ] = READ8( hsf, 0 );
		}
	}
	else {
		if( hsf == 0 ) {
			XBYTE[ REG_Cdsp_YUVscalesvden ] = 0x00;
			XBYTE[ 0x21c0 ] = 0x00;  		/* Disable RGB Hscale */
			hrmder = 0;
		}
		else {
			XBYTE[ REG_Cdsp_YUVscalesvden ] = 0x00;
			XBYTE[ 0x21c0 ] = 0x01;  		/* Disable RGB Hscale */

			hrmder = ( ( UINT32 ) 65536 ) % ( ( UINT32 ) hsf );

			XBYTE[ REG_Cdsp_YUVhscalefactor_low ]  = READ8( hsf, 1 );
			XBYTE[ REG_Cdsp_YUVhscalefactor_high ] = READ8( hsf, 0 );
			XBYTE[ REG_Cdsp_Yhscaleaccini_low ]  = READ8( hsf, 1 );
			XBYTE[ REG_Cdsp_Yhscaleaccini_high ] = READ8( hsf, 0 );
		}
	}

	roiYSize = (UINT32)dovsize * (UINT32)100 / (UINT32)vFactor;
	yOffset = ( dovsize - roiYSize ) >> 1;
	yOffset &= ~1;

	if( tgvsize >= roiYSize ) {
		vsf = 0;
	}
	else {
		factor1 = ( ( ( UINT32 ) tgvsize << 16 ) / ( UINT32 ) roiYSize );

		if( ( ( UINT32 ) 65536 ) % ( ( UINT32 ) factor1 ) != 0 ) {
			factor1++;
		}

		vsf = ( UINT16 ) factor1;
	}

	if( vsf == 0 ) {
		XBYTE[ REG_Cdsp_YUVvscaleen ] = 0;  		 /* Disable vscale */
	}
	else {
		XBYTE[ REG_Cdsp_YUVvscaleen ] = 0x11;

		XBYTE[ REG_Cdsp_YUVvscalefactor_low ]  = READ8( vsf, 1 );
		XBYTE[ REG_Cdsp_YUVvscalefactor_high ] = READ8( vsf, 0 );
		XBYTE[ REG_Cdsp_Yvscaleaccini_low ]  = READ8( vsf, 1 );
		XBYTE[ REG_Cdsp_Yvscaleaccini_high ] = READ8( vsf, 0 );
	}

	XBYTE[ REG_Cdsp_intplmirror ] &= 0xF0;

	if ( hmirroren == 1 ) {
		XBYTE[ REG_Cdsp_intplmirror ] |= 0x03;
	}

	if( mode != 2 ) {
		if( ( vmirroren == 1 ) ) {
			XBYTE[ REG_Cdsp_mirrorline ] = 0x32;
		}
		else {
			XBYTE[ REG_Cdsp_mirrorline ] = 0x00;
		}
	}
	else { // disable 0x21d0
		XBYTE[ REG_Cdsp_mirrorline ] = 0x00;
	}

	// adjust x size
	xOffset8 = xOffset & ~7;
	skip = xOffset - xOffset8;
	roiXSize8 = roiXSize + (UINT32)skip;
	roiXSize8 = ( roiXSize8 + 7 ) & ~7;

	XBYTE[ REG_Cdsp_Rhoffset_low ]  = READ8( xOffset8, 3 );
	XBYTE[ REG_Cdsp_Rhoffset_high ] = READ8( xOffset8, 2 );
	XBYTE[ REG_Cdsp_Rvoffset_low ]  = READ8( yOffset, 3 );
	XBYTE[ REG_Cdsp_Rvoffset_high ] = READ8( yOffset, 2 );
	XBYTE[ REG_Cdsp_Whoffset_low ]  = 0;
	XBYTE[ REG_Cdsp_Whoffset_high ] = 0;
	XBYTE[ REG_Cdsp_Wvoffset_low ]  = 0;
	XBYTE[ REG_Cdsp_Wvoffset_high ] = 0;
	XBYTE[ REG_Cdsp_Rhsize_low ]  = READ8( roiXSize8, 3 );
	XBYTE[ REG_Cdsp_Rhsize_high ] = READ8( roiXSize8, 2 );
	XBYTE[ REG_Cdsp_Rvsize_low ]  = READ8( roiYSize, 3 );
	XBYTE[ REG_Cdsp_Rvsize_high ] = READ8( roiYSize, 2 );

	XBYTE[ REG_Cdsp_Skippixel ] = READ8( skip, 1 );

#if 0
	printf("[Rhsize_low] = 0x%bx\n",XBYTE[REG_Cdsp_Rhsize_low]);
	printf("[Rhsize_high] = 0x%bx\n",XBYTE[REG_Cdsp_Rhsize_high]);
	printf("[Rvsize_low] = 0x%bx\n",XBYTE[REG_Cdsp_Rvsize_low]);
	printf("[Rvsize_high] = 0x%bx\n",XBYTE[REG_Cdsp_Rvsize_high]);
	printf("[Rhoffset_low] = 0x%bx\n",XBYTE[REG_Cdsp_Rhoffset_low]);
	printf("[Rhoffset_high] = 0x%bx\n",XBYTE[REG_Cdsp_Rhoffset_high]);
	printf("[Rvoffset_low] = 0x%bx\n",XBYTE[REG_Cdsp_Rvoffset_low]);
	printf("[Rvoffset_high] = 0x%bx\n",XBYTE[REG_Cdsp_Rvoffset_high]);
	printf("[Whoffset_low] = 0x%bx\n",XBYTE[REG_Cdsp_Whoffset_low]);
	printf("[Whoffset_high] = 0x%bx\n",XBYTE[REG_Cdsp_Whoffset_high]);
	printf("[Wvoffset_low] = 0x%bx\n",XBYTE[REG_Cdsp_Wvoffset_low]);
	printf("[Wvoffset_high] = 0x%bx\n",XBYTE[REG_Cdsp_Wvoffset_high]);
	printf("[Skippixel] = 0x%bx\n",XBYTE[REG_Cdsp_Skippixel]);
#endif

	if( hsf == 0 ) {
		m = roiXSize;
	}
	else {
		m = ( ( ( UINT32 ) roiXSize * ( UINT32 ) hsf ) >> 16 ) & ~7;
	}

	if( vsf == 0 ) {
		n = roiYSize;
	}
	else {
		n = ( ( ( UINT32 ) roiYSize * ( UINT32 ) vsf ) >> 16 ) & ~7;
	}

	HAL_CdspVDramSizeClamp( 1, ( UINT16 ) m, 1, ( UINT16 ) n, 0 );  /* Clamp the size to SDRAM */
	//printf(" m = %ld, n = %ld.\n",m,n);
	//printf("XBYTE[REG_Cdsp_Clamphsize_low] = 0x%bx\n",XBYTE[REG_Cdsp_Clamphsize_low]);
	//printf("XBYTE[REG_Cdsp_Clamphsize_high] = 0x%bx\n",XBYTE[REG_Cdsp_Clamphsize_high]);
	//printf("XBYTE[REG_Cdsp_Clampvsize_low] = 0x%bx\n",XBYTE[REG_Cdsp_Clampvsize_low]);
	//printf("XBYTE[REG_Cdsp_Clampvsize_high] = 0x%bx\n",XBYTE[REG_Cdsp_Clampvsize_high]);

	XBYTE[ 0x2121 ] &= ~0x10;  // lens disable

	XBYTE[ REG_Cdsp_evnt1 ] = 0xff;

#if 0
	//XBYTE[ 0x21f0 ] = 0x01;
	//XBYTE[ 0x21f0 ] = 0x00;
	HAL_CdspRstBlock(CDSPRstALL);
	printf( "CDSP reset\n" );
#endif

	
	
	HAL_CdspRegBackup();
	
	
	XBYTE[ REG_Cdsp_Remode ] = 0x10 | ( mode & 0x03 );   /* Set DoCDSP = 1 */
	//  		printf("21f0 = %bx \n",XBYTE[0x21f0]);
	//  		printf("2002 = %bx \n",XBYTE[0x2002]);

#if 1
	do {
		m = XBYTE[ REG_Cdsp_Remode ];   	 /* Wait DoCDSP */
		printf("wait for cdsp done\n");
	} while( ( m & 0x10 ) == 0x10 );
#else
	while( ( XBYTE[ REG_Cdsp_Remode ] & 0x10 ) == 0x10 ) {
		printf( "[0x%bx]", XBYTE[ REG_Cdsp_Remode ] );
	}
#endif
	//	printf("Call DoSub over \n");
	//  	printf("21f0 = %bx \n",XBYTE[0x21f0]);
	//    printf("2002 = %bx \n",XBYTE[0x2002]);

#if 0
	printf (" Front Input Size \n");
	printf (" 0x22F0 = %bx \n", XBYTE[0x22F0]);
	printf (" 0x22F1 = %bx \n", XBYTE[0x22F1]);
	printf (" 0x22F2 = %bx \n", XBYTE[0x22F2]);
	printf (" 0x22F3 = %bx \n", XBYTE[0x22F3]);
	printf (" RGB Hscale Output Size \n");
	printf (" 0x22F4 = %bx \n", XBYTE[0x22F4]);
	printf (" 0x22F5 = %bx \n", XBYTE[0x22F5]);
	printf (" 0x22F6 = %bx \n", XBYTE[0x22F6]);
	printf (" 0x22F7 = %bx \n", XBYTE[0x22F7]);
	printf (" YUV Mux Output Size \n");
	printf (" 0x22F8 = %bx \n", XBYTE[0x22F8]);
	printf (" 0x22F9 = %bx \n", XBYTE[0x22F9]);
	printf (" 0x22FA = %bx \n", XBYTE[0x22FA]);
	printf (" 0x22FB = %bx \n", XBYTE[0x22FB]);
	printf (" YUV Scale Output Size \n");
	printf (" 0x22FC = %bx \n", XBYTE[0x22FC]);
	printf (" 0x22FD = %bx \n", XBYTE[0x22FD]);
	printf (" 0x22FE = %bx \n", XBYTE[0x22FE]);
	printf (" 0x22FF = %bx \n", XBYTE[0x22FF]);
#endif

	//	   return L2K_SUCCESS;
	return 0;
}

/**
 * @fn        UINT8 HAL_CdspImgDo(UINT8 mode, UINT16 hoffset, UINT16 dohsize, UINT16 voffset, UINT16 dovsize, UINT8 hmirroren, UINT8 vmirroren, UINT16 dosubwidth, UINT16 dosubheight, UINT16 ScaleFactor)
 * @brief     Do Color processing from DRAM's raw data, and save YUV422 data to DRAM
 * @param     [in] mode : Source Image type,
 *							0 -> RGB 8 bit Mode,
 *							1 -> RGB 10 bit Mode,
 *							2 -> YUV Mode
 * @param     [in] hoffset : he horizontal image offset to start image do  (12 bits)
 * @param     [in] dohsize : the horizontal image size to do  (12 bits)
 * @param     [in] voffset : the voffset to start image do   (12 bits)
 * @param     [in] dovsize : the vertical size to do   (12 bits)
 * @param     [in] hmirroren : Horizontal mirror
 *								0 -> None mirror
 *								1 -> Start position mirror enable
 *								2 -> End position mirror enable
 *								3 -> Start & End position mirror enable
 * @param     [in] vmirroren : Vertical mirror
 *								0 -> None mirror
 *								1 -> Start position mirror enable
 *								2 -> End position mirror enable
 *								3 -> Start & End position mirror enable
 * @param     [in] dosubwidth
 * @param     [in] dosubheight
 * @param     [in] ScaleFactor : Vertical scale factor   (16 bits)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspImgDo(
	UINT8 mode,
	UINT16 hoffset,
	UINT16 dohsize,
	UINT16 voffset,
	UINT16 dovsize,
	UINT8 hmirroren,
	UINT8 vmirroren,
	UINT16 dosubwidth,
	UINT16 dosubheight,
	UINT16 ScaleFactor
)
{
	UINT16 i, j;
	UINT8 VSub, HSub;

	ScaleFactor = 0;
	XBYTE[ 0x2276 ] = 0x00;
	XBYTE[ 0x226E ] = 0x88;
	XBYTE[ 0x226f ] = 0x00;

	if( ( XBYTE[ 0x21D0 ] & 0x0f ) != 0 ) {
		XBYTE[ 0x21D0 ] = ( XBYTE[ 0x21D0 ] | 0x10 ) ;
		if( mode != 0 ) {
			VSub = 2;
		}
		else {
			VSub = 4;
		}

		HSub = 8;
	}
	else {
		XBYTE[ 0x21D0 ] = 0;
		if( mode != 0 ) {
			VSub = 0;
			HSub = 0;
		}
		else {
			VSub = 2;
			HSub = 8;
		}
	}

	if( dohsize <= dosubwidth && dovsize <= dosubheight ) {
		HAL_CdspImgDoSub( mode, hoffset, dohsize, voffset, dovsize, hmirroren, vmirroren, 0, 0, hoffset + HSub, 0, 0, (UINT8)( voffset % 2 ), 0 );
	}
	else {
		if( dohsize <= dosubwidth ) {
			HAL_CdspImgDoSub( mode, hoffset, dohsize, voffset, dovsize, hmirroren, vmirroren, 0, 0, hoffset + HSub, 0, 0, (UINT8)( voffset % 2 ), 0 );
		}
		else {
			if( dovsize <= dosubheight ) {
				HAL_CdspImgDoSub( mode, 0, dosubwidth, voffset, dovsize, 1, vmirroren, 0, 0, HSub, 0, 0, (UINT8)( voffset % 2 ), 0 );
				for( i = dosubwidth - HSub; i < ( dohsize - dosubwidth - HSub ); i = i + dosubwidth ) {
					HAL_CdspImgDoSub( mode, i, dosubwidth, voffset, dovsize, 0, vmirroren, 0, 0, i + HSub, 0, 0, (UINT8)( voffset % 2 ), 0 );
				}
				HAL_CdspImgDoSub( mode, i, dohsize - i - HSub, voffset, dovsize, 2, vmirroren, 0, 0, i + HSub, 0, 0, (UINT8)( voffset % 2 ), 0 );
			}
			else {
				HAL_CdspImgDoSub( mode, 0, dosubwidth, 0, dosubheight, 1, 1, 0, 0, HSub, 0, 0, 0, 0 ); // (1,1)

				for( i = dosubwidth - HSub; i < ( dohsize - dosubwidth - HSub ); i = i + dosubwidth ) {
					HAL_CdspImgDoSub( mode, i, dosubwidth, 0, dosubheight, 0, 1, 0, 0, i + HSub, 0, 0, 0, 0 );  // (1,2)
				}

				HAL_CdspImgDoSub( mode, i, dohsize - i - HSub, 0, dosubheight, 2, 1, 0, 0, i + HSub, 0, 0, 0, 0 );

				for( j = dosubheight - VSub; j < ( dovsize - dosubheight - VSub ); j = j + dosubheight ) {
					HAL_CdspImgDoSub( mode, 0, dosubwidth, j, dosubheight, 1, 0, 0, 0, HSub, j + VSub, 0, (UINT8)( j % 2 ), 0 );
					for( i = dosubwidth - HSub; i < ( dohsize - dosubwidth - HSub ); i = i + dosubwidth ) {
						HAL_CdspImgDoSub( mode, i, dosubwidth, j, dosubheight, 0, 0, 0, 0, i + HSub, j + VSub, 0, (UINT8)( j % 2 ), 0 );
					}

					HAL_CdspImgDoSub( mode, i, dohsize - i - HSub, j, dosubheight, 2, 0, 0, 0, i + HSub, j + VSub, 0, (UINT8)( j % 2 ), 0 );
				}

				HAL_CdspImgDoSub( mode, 0, dosubwidth, j, dovsize - j - VSub, 1, 2, 0, 0, HSub, j + VSub, 0, (UINT8)( j % 2 ), 0 );

				for( i = dosubwidth - HSub; i < ( dohsize - dosubwidth - HSub ); i = i + dosubwidth ) {
					HAL_CdspImgDoSub( mode, i, dosubwidth, j, dovsize - j - VSub, 0, 2, 0, 0, i + HSub, j + VSub, 0, (UINT8)( j % 2 ), 0 );
				}

				HAL_CdspImgDoSub( mode, i, dohsize - i - HSub, j, dovsize - j - VSub, 2, 2, 0, 0, i + HSub, j + VSub, 0, (UINT8)( j % 2 ), 0 );
			}
		}
	}

	return 0;
}

/**
 * @fn        UINT8 HAL_CdspImgDoSub(UINT8 mode, UINT16 hoffset, UINT16 dohsize, UINT16 voffset, UINT16 dovsize, UINT8 hmirroren, UINT8 vmirroren, UINT16 hfactor, UINT16 vfactor, UINT16 writeHOffset, UINT16 writeVOffset, UINT8 number, UINT8 pixel, UINT32 vrmder)
 * @brief     HAL_CdspImgDoSub
 * @param     [in] mode
 * @param     [in] hoffset
 * @param     [in] dohsize
 * @param     [in] voffset
 * @param     [in] dovsize
 * @param     [in] hmirroren
 * @param     [in] vmirroren
 * @param     [in] hfactor
 * @param     [in] vfactor
 * @param     [in] writeHOffset
 * @param     [in] writeVOffset
 * @param     [in] number
 * @param     [in] pixel
 * @param     [in] vrmder
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/

UINT8
HAL_CdspImgDoSub(
	UINT8  mode,
	UINT16 hoffset,
	UINT16 dohsize,
	UINT16 voffset,
	UINT16 dovsize,
	UINT8  hmirroren,
	UINT8  vmirroren,
	UINT16 hfactor,
	UINT16 vfactor,
	UINT16 writeHOffset,
	UINT16 writeVOffset,
	UINT8 number,
	UINT8 pixel,
	UINT32 vrmder
)
{
	#ifndef HAL_CDSP_CORE_TAG_TODO

	UINT16 tmp0, wHClampSize, wVClampSize;
	UINT8 yuvCoringBlock;

	if( ( XBYTE[ 0x21d0 ] & 0x0f ) != 0x00 ) {
		yuvCoringBlock = 1;
	}
	else {
		yuvCoringBlock = 0;
	}

	XBYTE[ REG_Cdsp_HRGBscaleen ] = 0;  		  /* Disable RGB hscale */
	if( hfactor == 0 ) {
		XBYTE[ REG_Cdsp_YUVhscaleen ] = 0;  		/* Disable YUV Hscale */
	}
	else {
		XBYTE[ REG_Cdsp_YUVhscaleen ] = 0x11;
		XBYTE[ REG_Cdsp_YUVhscalefactor_low ] = LowByte( hfactor );
		XBYTE[ REG_Cdsp_YUVhscalefactor_high ] = HighByte( hfactor );
		XBYTE[ REG_Cdsp_Yhscaleaccini_low ] = LowByte( hfactor );
		XBYTE[ REG_Cdsp_Yhscaleaccini_high ] = HighByte( hfactor );
	}

	if( vfactor == 0 ) {
		XBYTE[ REG_Cdsp_YUVvscaleen ] = 0;  		 /* Disable vscale */
	}
	else {
		XBYTE[ REG_Cdsp_YUVvscaleen ] = 0x11;
		XBYTE[ REG_Cdsp_YUVvscalefactor_low ] = LowByte( vfactor );
		XBYTE[ REG_Cdsp_YUVvscalefactor_high ] = HighByte( vfactor );
		XBYTE[ REG_Cdsp_Yvscaleaccini_low ] = LowByte( vfactor );
		XBYTE[ REG_Cdsp_Yvscaleaccini_high ] = HighByte( vfactor );
	}

	if( ( XBYTE[ 0x21d0 ] & 0x0f ) != 0x00 ) {
		if( hmirroren == 0 ) {
			if ( mode <= 1 ) {
				XBYTE[ REG_Cdsp_Skippixel ] = 0x04;  //Source RGB
			}
			else {
				XBYTE[ REG_Cdsp_Skippixel ] = 0x06;  //Source YUV
			}
			XBYTE[ REG_Cdsp_intplmirror ] &= 0xF0;
			XBYTE[ REG_Cdsp_EsmirEn ] &= 0xF0;

//			tmp0 = dohsize + 16;
			tmp0 = dohsize + ( ( cdspYuvCoringTime - number + 1 ) << 4 );

			wHClampSize = tmp0 - 16;
		}
		else if( hmirroren == 1 ) {
			XBYTE[ REG_Cdsp_Skippixel ] = 0x00;
			XBYTE[ REG_Cdsp_intplmirror ] = ( XBYTE[ REG_Cdsp_intplmirror ] & 0xF0 ) | 0x01;
			XBYTE[ REG_Cdsp_EsmirEn ] = ( XBYTE[ REG_Cdsp_EsmirEn ] & 0xF0 ) | 0x01;

//			tmp0 = dohsize + 8;
			tmp0 = dohsize + ( ( cdspYuvCoringTime - number + 1 ) << 3 );

			wHClampSize = tmp0 - 8;
		}
		else if( hmirroren == 2 ) {
			if ( mode <= 1 ) {
				XBYTE[ REG_Cdsp_Skippixel ] = 0x04;  //Source RGB
			}
			else {
				XBYTE[ REG_Cdsp_Skippixel ] = 0x06;  //Source YUV
			}

			XBYTE[ REG_Cdsp_intplmirror ] = ( XBYTE[ REG_Cdsp_intplmirror ] & 0xF0 ) | 0x02;
			XBYTE[ REG_Cdsp_EsmirEn ] = ( XBYTE[ REG_Cdsp_EsmirEn ] & 0xF0 ) | 0x02;

//			tmp0 = dohsize + 8;
			tmp0 = dohsize + ( ( cdspYuvCoringTime - number + 1 ) << 3 );

			wHClampSize = tmp0 - 8;
		}
		else if( hmirroren == 3 ) {
			XBYTE[ REG_Cdsp_Skippixel ] = 0x00;
			XBYTE[ REG_Cdsp_intplmirror ] = ( XBYTE[ REG_Cdsp_intplmirror ] & 0xF0 ) | 0x03;
			XBYTE[ REG_Cdsp_EsmirEn ] = ( XBYTE[ REG_Cdsp_EsmirEn ] & 0xF0 ) | 0x03;

			tmp0 = dohsize;

			wHClampSize = tmp0;
		}
	}
	else {	// 0x21d0 = 0
		if( hmirroren == 0 ) {
			if ( mode <= 1 ) {
				XBYTE[ REG_Cdsp_Skippixel ] = 0x06;  //Source RGB
			}
			else {
				XBYTE[ REG_Cdsp_Skippixel ] = 0x00;  //Source YUV
			}

			XBYTE[ REG_Cdsp_intplmirror ] &= 0xF0;
			XBYTE[ REG_Cdsp_EsmirEn ] &= 0xF0;

			if( mode <= 1 ) {
				tmp0 = dohsize + 16;
//				tmp0 = dohsize + 16 * ( cdspYuvCoringTime - number + 1 );

				wHClampSize = tmp0 - 16;
			}
			else {
				tmp0 = dohsize;

				wHClampSize = tmp0;
			}
		}
		else if( hmirroren == 1 ) {
			XBYTE[ REG_Cdsp_Skippixel ] = 0x00;
			XBYTE[ REG_Cdsp_intplmirror ] = ( XBYTE[ REG_Cdsp_intplmirror ] & 0xF0 ) | 0x01;
			XBYTE[ REG_Cdsp_EsmirEn ] &= 0xF0;

			if( mode <= 1 ) {
				tmp0 = dohsize + 8;
//				tmp0 = dohsize + 8 * ( cdspYuvCoringTime - number + 1 );

				wHClampSize = tmp0 - 8;
			}
			else {
				tmp0 = dohsize;

				wHClampSize = tmp0;
			}
		}
		else if( hmirroren == 2 ) {
			if ( mode <= 1 ) {
				XBYTE[ REG_Cdsp_Skippixel ] = 0x06;  //Source RGB
			}
			else {
				XBYTE[ REG_Cdsp_Skippixel ] = 0x00;  //Source YUV
			}

			XBYTE[ REG_Cdsp_intplmirror ] = ( XBYTE[ REG_Cdsp_intplmirror ] & 0xF0 ) | 0x02;
			XBYTE[ REG_Cdsp_EsmirEn ] &= 0xF0;

			if( mode <= 1 ) {
				tmp0 = dohsize + 8;
//				tmp0 = dohsize + 8 * ( cdspYuvCoringTime - number + 1 );

				wHClampSize = tmp0 - 8;
			}
			else {
				tmp0 = dohsize;
//				tmp0 = dohsize + 8 * ( cdspYuvCoringTime - number + 1 );

				wHClampSize = tmp0;
			}
		}
		else if( hmirroren == 3 ) {
			XBYTE[ REG_Cdsp_Skippixel ] = 0x00;
			XBYTE[ REG_Cdsp_intplmirror ] = ( XBYTE[ REG_Cdsp_intplmirror ] & 0xF0 ) | 0x03;
			XBYTE[ REG_Cdsp_EsmirEn ] &= 0xF0;

			tmp0 = dohsize;

			wHClampSize = tmp0;
		}
	}

	XBYTE[ REG_Cdsp_Rhoffset_low ] = LowByte( hoffset );
	XBYTE[ REG_Cdsp_Rhoffset_high ] = HighByte( hoffset );
	XBYTE[ REG_Cdsp_Rhsize_low ] = LowByte( tmp0 );
	XBYTE[ REG_Cdsp_Rhsize_high ] = HighByte( tmp0 );

	XBYTE[ REG_Cdsp_Whoffset_low ] = LowByte( writeHOffset );
	XBYTE[ REG_Cdsp_Whoffset_high ] = HighByte( writeHOffset );

	if( ( XBYTE[ 0x21d0 ] & 0x0f ) != 0x00 ) {
		if( vmirroren == 0 ) {
			if ( mode <= 1 ) {
				XBYTE[ REG_Cdsp_mirrorline ] = 0x04;
//				tmp0 = dovsize + 8;
			}
			else {
				XBYTE[ REG_Cdsp_mirrorline ] = 0x02;
//				tmp0 = dovsize + 4;
			}

			tmp0 = dovsize + ( ( cdspYuvCoringTime - number + 1 ) << 2 );

			if( vrmder == 0 ) {
				wVClampSize = tmp0 - 4;
				if( mode <= 1 ) {
					tmp0 += 4;
				}
			}
			else {
				wVClampSize = tmp0 - 3;
				if( mode <= 1 ) {
					tmp0 += 5;
				}
				else {
					tmp0++;
				}
			}

			if( pixel == 0 ) {
				XBYTE[ REG_Cdsp_pixsel ] = halCdspBayerPtn;
			}
			else {
				XBYTE[ REG_Cdsp_pixsel ] = ( halCdspBayerPtn & 0xFD ) | ( halCdspBayerPtn ^ 0x02 );
			}
		}
		else if( vmirroren == 1 ) {
			if ( mode <= 1 ) {
				XBYTE[ REG_Cdsp_mirrorline ] = 0x24;
//				tmp0 = dovsize + 4;
			}
			else {
				XBYTE[ REG_Cdsp_mirrorline ] = 0x22;
//				tmp0 = dovsize + 2;
			}

			tmp0 = dovsize + ( ( cdspYuvCoringTime - number + 1 ) << 1 );

			wVClampSize = tmp0 - 2;
			if( mode <= 1 ) {
				tmp0 += 2;
			}

			XBYTE[ REG_Cdsp_pixsel ] = halCdspBayerPtn;
		}
		else if( vmirroren == 2 ) {
			if ( mode <= 1 ) {
				XBYTE[ REG_Cdsp_mirrorline ] = 0x14;
//				tmp0 = dovsize + 4;
			}
			else {
				XBYTE[ REG_Cdsp_mirrorline ] = 0x12;
//				tmp0 = dovsize + 2;
			}

			tmp0 = dovsize + ( ( cdspYuvCoringTime - number + 1 ) << 1 );

			if( vrmder == 0 ) {
				wVClampSize = tmp0 - 2;
				if( mode <= 1 ) {
					tmp0 += 2;
				}
			}
			else {
				wVClampSize = tmp0 - 1;
				if( mode <= 1 ) {
					tmp0 += 3;
				}
				else {
					tmp0++;
				}
			}

			if( pixel == 0 ) {
				XBYTE[ REG_Cdsp_pixsel ] = halCdspBayerPtn;
			}
			else {
				XBYTE[ REG_Cdsp_pixsel ] = ( halCdspBayerPtn & 0xFD ) | ( halCdspBayerPtn ^ 0x02 );
			}
		}
		else if( ( vmirroren == 3 ) ) {
			if ( mode <= 1 ) {
				XBYTE[ REG_Cdsp_mirrorline ] = 0x34;
			}
			else {
				XBYTE[ REG_Cdsp_mirrorline ] = 0x32;
			}

			tmp0 = dovsize;

			wVClampSize = tmp0;

			XBYTE[ REG_Cdsp_pixsel ] = halCdspBayerPtn;
		}
	}
	else {	// 0x21d0 = 0
		tmp0 = dovsize;
		wVClampSize = tmp0;
		if( vmirroren == 0 ) {
			if ( mode <= 1 ) {
				XBYTE[ REG_Cdsp_mirrorline ] = 0x02;
//				tmp0 = dovsize + 8;
			}
			else {
				XBYTE[ REG_Cdsp_mirrorline ] = 0x00;
//				tmp0 = dovsize + 4;
			}

//			tmp0 = dovsize + 4 * ( cdspYuvCoringTime - number + 1 );

			if( mode <= 1 ) {
				tmp0 += 4;

				if( pixel == 0 ) {
					XBYTE[ REG_Cdsp_pixsel ] = halCdspBayerPtn;
				}
				else {
					XBYTE[ REG_Cdsp_pixsel ] = ( halCdspBayerPtn & 0xFD ) | ( halCdspBayerPtn ^ 0x02 );
				}
			}
		}
		else if( vmirroren == 1 ) {
			if ( mode <= 1 ) {
				XBYTE[ REG_Cdsp_mirrorline ] = 0x22;
//				tmp0 = dovsize + 4;
			}
			else {
				XBYTE[ REG_Cdsp_mirrorline ] = 0x00;
//				tmp0 = dovsize + 2;
			}

//			tmp0 = dovsize + 2 * ( cdspYuvCoringTime - number + 1 );

			if( mode <= 1 ) {
				tmp0 += 2;
			}

			XBYTE[ REG_Cdsp_pixsel ] = halCdspBayerPtn;
		}
		else if( vmirroren == 2 ) {
			if ( mode <= 1 ) {
				XBYTE[ REG_Cdsp_mirrorline ] = 0x12;
//				tmp0 = dovsize + 4;
			}
			else {
				XBYTE[ REG_Cdsp_mirrorline ] = 0x00;
//				tmp0 = dovsize + 2;
			}

//			tmp0 = dovsize + 2 * ( cdspYuvCoringTime - number + 1 );

			if( mode <= 1 ) {
				tmp0 += 2;

				if( pixel == 0 ) {
					XBYTE[ REG_Cdsp_pixsel ] = halCdspBayerPtn;
				}
				else {
					XBYTE[ REG_Cdsp_pixsel ] = ( halCdspBayerPtn & 0xFD ) | ( halCdspBayerPtn ^ 0x02 );
				}
			}
		}
		else if( ( vmirroren == 3 ) ) {
			if ( mode <= 1 ) {
				XBYTE[ REG_Cdsp_mirrorline ] = 0x32;
			}
			else {
				XBYTE[ REG_Cdsp_mirrorline ] = 0x00;
			}

			XBYTE[ REG_Cdsp_pixsel ] = halCdspBayerPtn;
		}
	}

	XBYTE[ REG_Cdsp_Rvoffset_low ]  = READ8( voffset, 1 );
	XBYTE[ REG_Cdsp_Rvoffset_high ] = READ8( voffset, 0 );

	XBYTE[ REG_Cdsp_Rvsize_low ]  = READ8( tmp0, 1 );
	XBYTE[ REG_Cdsp_Rvsize_high ] = READ8( tmp0, 0 );

	tmp0 = writeVOffset;

	XBYTE[ REG_Cdsp_Wvoffset_low ]  = READ8( tmp0, 1 );
	XBYTE[ REG_Cdsp_Wvoffset_high ] = READ8( tmp0, 0 );
	HAL_CdspVDramSizeClamp( 1, wHClampSize, 1, wVClampSize, 0 );  /* Clamp the size to SDRAM */

//	XBYTE[ 0x2262 ] = 0xff;
//	XBYTE[ REG_Cdsp_evnt1 ] = 0xff;

	HAL_CdspRegBackup();
	
	
	XBYTE[ REG_Cdsp_Remode ] = 0x10 | ( mode & 0x03 );   /* Set DoCDSP = 1 */

	do {
		tmp0 = XBYTE[ REG_Cdsp_Remode ];		/* Wait DoCDSP */
		/*  printf("before DO DO \n");
		  for (i=0;i<0x200 ;i++)
		  {
			printf("XBYTE[0x%x]=0x%bx \n",REG_Cdsp_macroSel+i,XBYTE[REG_Cdsp_macroSel+i]);
		  }

		  for (i=0;i<0x100 ;i++)
		  {
			printf("XBYTE[0x%x]=0x%bx \n",0x2700+i,XBYTE[0x2700+i]);
		  }

		  printf (" 0x21ee = %bx \n", XBYTE[REG_Cdsp_evnt1]);*/


		//printf ("0x2260 = %bx \n", XBYTE[0x2260]);
	} while( ( tmp0 & 0x10 ) == 0x10 );

	#if 0
		printf (" 0x21ee = %bx \n", XBYTE[REG_Cdsp_evnt1]);
		printf (" XBYTE[REG_Cdsp_Remode] = %bx \n", XBYTE[REG_Cdsp_Remode]);
		printf (" 0x2130 = %bx \n", XBYTE[0x2130]);
		printf (" Front Input Size \n");
		printf (" 0x22F0 = %bx \n", XBYTE[0x22F0]);
		printf (" 0x22F1 = %bx \n", XBYTE[0x22F1]);
		printf (" 0x22F2 = %bx \n", XBYTE[0x22F2]);
		printf (" 0x22F3 = %bx \n", XBYTE[0x22F3]);
		printf (" RGB Hscale Output Size \n");
		printf (" 0x22F4 = %bx \n", XBYTE[0x22F4]);
		printf (" 0x22F5 = %bx \n", XBYTE[0x22F5]);
		printf (" 0x22F6 = %bx \n", XBYTE[0x22F6]);
		printf (" 0x22F7 = %bx \n", XBYTE[0x22F7]);
		printf (" YUV Mux Output Size \n");
		printf (" 0x22F8 = %bx \n", XBYTE[0x22F8]);
		printf (" 0x22F9 = %bx \n", XBYTE[0x22F9]);
		printf (" 0x22FA = %bx \n", XBYTE[0x22FA]);
		printf (" 0x22FB = %bx \n", XBYTE[0x22FB]);
		printf (" YUV Scale Output Size \n");
		printf (" 0x22FC = %bx \n", XBYTE[0x22FC]);
		printf (" 0x22FD = %bx \n", XBYTE[0x22FD]);
		printf (" 0x22FE = %bx \n", XBYTE[0x22FE]);
		printf (" 0x22FF = %bx \n", XBYTE[0x22FF]);
	#endif
	#else
		mode=mode;
		hoffset=hoffset;
		dohsize=dohsize;
		voffset=voffset;
		dovsize=dovsize;
		hmirroren=hmirroren;
		vmirroren=vmirroren;
		hfactor=hfactor;
		vfactor=vfactor;
		writeHOffset=writeHOffset;
		writeVOffset=writeVOffset;
		number=number;
		pixel=pixel;
		vrmder=vrmder;
	
	#endif
	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspScaleSyncModeSet(UINT8 scalesvden, UINT8 scaleszfen)
 * @brief     CDSP scale engine sync mode
 * @param     [in] scalesvden : 
 *								0: immediate update
 *								1: reflected at the next valid VD edge
 * @param     [in] scaleszfen :
 *								0: depend on scalesvden
 *								1: reflected at the zoomfactor update(1 bit)
 *
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspScaleSyncModeSet(
	UINT8 scalesvden,
	UINT8 scaleszfen
)
{
	XBYTE[ REG_Cdsp_YUVscalesvden ] = ( scaleszfen & 0x01 ) << 4 | ( scalesvden & 0x01 );

	return HALK_SUCCESS;
}

#endif
