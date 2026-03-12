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
 * @file      Hal_cdsp
 * @brief     Brief
 * @author    Author's name
 * @since     2010-01-01
 * @date      2010-01-01
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
#include "hal_front.h"
#include "hal_dram.h"
#include "reg_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define _DBG_WITH_ALL_FUNC_

#define HAL_CDSP_REMOVE_UNCALL_FUNCTION
#define HAL_CDSP_AF_FUNCTION   

#define __FILE	__FILE_ID_HAL_CDSP__

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define HighByte(x) 	(unsigned char) ((x) >> 8)
#define LowByte(x)  	(unsigned char) (x)
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/*
#ifndef FPGA_MEM_POOL_ADDR
	xdata	raw_pixel_t AWBBuff[ 1024 ] _at_ 0x4000;
#else
	xdata	raw_pixel_t AWBBuff[ 1024 ];
#endif

typedef struct AWB_Calvalue {
	UINT16 R_Value;
	UINT16 G_Value;
	UINT16 B_Value;
} AWB_valuet, * pAWB_resultvalue; 
*/	
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
xdata UINT32 snapCdspRegAddr;
xdata UINT16 snapCdspRegSize;
xdata UINT16 snapCdspRegCnt;		
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
		
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**
 * @fn        UINT8 HAL_CdspCropSet(UINT8 crop_henable, UINT8 crop_venable, UINT8 crop_sync_enable, UINT16 crop_hoffset, UINT16 crop_hsize, UINT16 crop_voffset, UINT16 crop_vsize)
 * @brief     To set the Crop offset for CDSP 
 * @param     [in] crop_henable : Enable horizontal image cropping (1 bit)
 * @param     [in] crop_venable : Enable vertical image cropping (1 bit)
 * @param     [in] crop_sync_enable : Crop sync enable method (2 bit)
 *										0: immediate update
 *										1: reflected at vdupdate & VD falling edge
 *										2: Crop sync zoomfactor enable 
 *				(crop_sync_vd_enable: in, Written cropen & cropoffset/size register value update (1 bit)
 *										0: immediate update
 *										1: reflected at vdupdate & VD falling edge
 * @param     [in] crop_hoffset : the crop hoffset (12 bit)
 * @param     [in] crop_hsize : the crop voffset (12 bit)
 * @param     [in] crop_voffset : the crop hsize (12 bit)
 * @param     [in] crop_vsize : the crop vsize (12 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspCropSet(
	UINT8 crop_henable,
	UINT8 crop_venable,
	UINT8 crop_sync_enable,
	UINT16 crop_hoffset,
	UINT16 crop_hsize,
	UINT16 crop_voffset,
	UINT16 crop_vsize
)
{

	#if 1
		XBYTE[ REG_Cdsp_Cropen ] = ( ( crop_venable & 0x01 ) << 1 ) | ( crop_henable & 0x01 ) | ((crop_sync_enable&0x03)<<4);
	#else
		if( crop_sync_enable >= 2 ) {
			XBYTE[ REG_Cdsp_Cropen ] |= 0x20;
		}
		else {
			XBYTE[ REG_Cdsp_Cropen ] |= ( crop_sync_enable & 0x01 ) << 4;
		}

		XBYTE[ REG_Cdsp_Cropen ] &= 0xfc;
		XBYTE[ REG_Cdsp_Cropen ] |= ( ( crop_venable & 0x01 ) << 1 ) | ( crop_henable & 0x01 );
	#endif

	//printf("crop HSize=%d,off=%d\n",crop_hsize,crop_hoffset);

	XBYTE[ REG_Cdsp_pcrophoffset_low ] = LowByte( crop_hoffset );
	XBYTE[ REG_Cdsp_pcrophoffset_high ] = HighByte( crop_hoffset );

	XBYTE[ REG_Cdsp_pcrophsize_low ] = LowByte( crop_hsize );
	XBYTE[ REG_Cdsp_pcrophsize_high ] = HighByte( crop_hsize );

	XBYTE[ REG_Cdsp_pcropvoffset_low ] = LowByte( crop_voffset );
	XBYTE[ REG_Cdsp_pcropvoffset_high ] = HighByte( crop_voffset );

	XBYTE[ REG_Cdsp_pcropvsize_low ] = LowByte( crop_vsize );
	XBYTE[ REG_Cdsp_pcropvsize_high ] = HighByte( crop_vsize );

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspHScaleRgbSet(UINT8 hscale_enable, UINT8 hscale_mode, UINT8 hscale_sync_mode, UINT16 shsize, UINT16 dhsize)
 * @brief     Set CDSP horizontal scale down factor in RGB domain. 
 * @param     [in] hscale_enable : the horizontal scale enable/disable.(1 bit)
 * @param     [in] hscale_mode : Horizontal raw data scale down mode (1 bit)
 * @param     [in] hscale_sync_mode : in,hscale sync method (2 bit)
 *										0: immediate update
 *										1: reflected at the next valid VD edge
 *										2: reflected at the zoomfactor update
 * @param     [in] shsize : the horizontal size of source image
 * @param     [in] dhsize : the horizontal size of destination image
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspHScaleRgbSet(
	UINT8 hscale_enable,
	UINT8 hscale_mode,
	UINT8 hscale_sync_mode,
	UINT16 shsize,
	UINT16 dhsize
)
{
	UINT32 factor1;
	UINT16 factor;
	XBYTE[ REG_Cdsp_HRGBscaleen ] = ( ( hscale_mode & 0x01 ) << 4 ) | ( hscale_enable & 0x01 );

	HAL_CdspSyncModeSet(hscale_sync_mode);
	
	if( dhsize >= shsize ) {
		XBYTE[ REG_Cdsp_HRGBscalefactor_low ] = 0;
		XBYTE[ REG_Cdsp_HRGBscalefactor_high ] = 0;
		XBYTE[ REG_Cdsp_HRGBscaleen ] = 0;
	}
	else {
		factor1 = ( ( ( UINT32 ) dhsize << 16 ) / ( UINT32 ) shsize ) + 1;
		factor = ( UINT16 ) factor1;

		XBYTE[ REG_Cdsp_HRGBscaleinieve_low ] = LowByte( ( factor / 2 ) + 0x8000 );
		XBYTE[ REG_Cdsp_HRGBscaleinieve_high ] = HighByte( ( factor / 2 ) + 0x8000 );

		XBYTE[ REG_Cdsp_HRGBscaleiniodd_low ] = LowByte( factor );
		XBYTE[ REG_Cdsp_HRGBscaleiniodd_high ] = HighByte( factor );

		XBYTE[ REG_Cdsp_HRGBscalefactor_low ] = LowByte( factor );
		XBYTE[ REG_Cdsp_HRGBscalefactor_high ] = HighByte( factor );

		//		XBYTE[REG_Cdsp_HRGBscaleen] =0x11;
	}

	return HALK_SUCCESS ;
}

/**
 * @fn        void HAL_CdspWriteBufIdxSet(UINT8 bufmode)
 * @brief     Set CDSP write index
 * @param     [in] bufIdx (2 bits)
 *								0: MPG Source 0
 *								1: MPG Source 1
 *								2: MPG Source 2
 *								3: MPG Source 3
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
void 
HAL_CdspWriteBufIdxSet(
	UINT8  bufIdx
)
{
	
	XBYTE[REG_Dram_MpgSb_CdspWRegIdx] &= 0xfc;
	
	switch (bufIdx) {
		case 0: 
    		XBYTE[REG_Dram_MpgSb_CdspWRegIdx]  |= 0x00;	 /* CDSP Write MPG Source 0 */
    		break;
		case 1: 
    	    XBYTE[REG_Dram_MpgSb_CdspWRegIdx]  |= 0x01;	 /* CDSP Write MPG Source 1 */
    	    break;
		case 2: 
    	    XBYTE[REG_Dram_MpgSb_CdspWRegIdx]  |= 0x02;	 /* CDSP Write MPG Source 2 */
    	    break;
		case 3: 
    	    XBYTE[REG_Dram_MpgSb_CdspWRegIdx]  |= 0x03;	 /* CDSP Write MPG Source 3 */
    	    break;
	}

	XBYTE[REG_Dram_MpgSb_RegIdx_En] |= 0x02;	 /* Enable CDSP Write Index Register */

}


/**
 * @fn        UINT8 HAL_CdspWriteBufIdxGet(void)
 * @brief     HAL_CdspWriteBufIdxGet
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-17
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
HAL_CdspWriteBufIdxGet(
	void
)
{	
	UINT8 idx;

 	idx= XBYTE[REG_Dram_MpgSb_CdspWRegIdx] & 0x03;

	return idx;
}


/**
 * @fn        void HAL_CdspFrmCtrlEnSet(UINT8 en)
 * @brief     enable/disable frame control
 * @param     [in] en : 0: disable , 1: enable
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-17
 * @todo      N/A
 * @bug       N/A
*/
void 
HAL_CdspFrmCtrlEnSet(
	UINT8 en
)
{
	XBYTE[REG_Dram_FrcEn] = (en&0x01);
}

/**
 * @fn        UINT8 HAL_CdspClockSet(UINT8 src, UINT8 clkdiv)
 * @brief     Set CDSP clock source
 * @param     [in] src : 
 *						0: front 1x
 *						1: divide from system PLL
 * @param     [in] clkdiv : clock divider						
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_CdspClockSet(
	UINT8 src,
	UINT8 clkdiv
)
{

	XBYTE[ 0x2019 ] &= 0xbf;
    XBYTE[ 0x2019 ] |= ((src & 0x01)<<6); /* CDSP Clock selcetion */

	XBYTE[ 0x2022 ] =  (clkdiv & 0x0f);             	   /* CDSP Clock divider */
	
    //XBYTE[ 0x2019 ] |= 0x40;    /* CDSP Clock selection*/   
	//XBYTE[ 0x2022 ] = 0x00;     //CDSP Clock divider
	//XBYTE[0x2027] = 0x00;   //Front nx clock divider
	return HALK_SUCCESS;
}


/**
 * @fn        UINT8 HAL_CdspRstBlock(UINT8 CDSPrstidx)
 * @brief     Reset CDSP module
 * @param     [in] CDSPrstidx : 
 *								0: reset all
 *								1: reset AE Window
 *								2: reset AWB Window
 *								3: reset Edge Window
 *								4: reset DramV
 *								5: reset DramV1
 *								6: reset VDram
 *								7: reset VDram1
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
HAL_CdspRstBlock(
	UINT8 cdspRstIdx
)
{
    //ASSERT(cdspRstIdx<=7,0);
	if (cdspRstIdx>7)
		return HALK_ERROR_PARAMETER;
	
    XBYTE[REG_Cdsp_rstcdsp]=(0x01 << cdspRstIdx);
	XBYTE[REG_Cdsp_rstcdsp]=0x00;

    return HALK_SUCCESS;
}



/**
 * @fn        UINT8 HAL_CdspDramVMirrorSet(UINT8 mirrorline, UINT8 Vsmirroren, UINT8 Vemirroren)
 * @brief     Set CDSP vertical mirror function while doing CDSP
 * @param     [in] mirrorline : The line number to mirror (0~15)
 * @param     [in] Vsmirroren : The top side mirror enable
 * @param     [in] Vemirroren : The bottom side mirror enable
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspDramVMirrorSet(
	UINT8 mirrorLine,
	UINT8 vsMirrorEn,
	UINT8 veMirrorEn
)
{
	XBYTE[ REG_Cdsp_mirrorline ] = ( ( vsMirrorEn & 0x01 ) << 5 ) | ( ( veMirrorEn & 0x01 ) << 4 ) | ( mirrorLine & 0x0f );

	return HALK_SUCCESS;
}


/**
 * @fn        UINT8 HAL_CdspDramVSkippixelSet(UINT8 Skippixel)
 * @brief     Set number of pixels that will be sikpped
 * @param     [in] Skippixel : The Skip pixel when read data from SDRAM (range 0~7)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspDramVSkippixelSet(
	UINT8 skipPixel
)
{
	XBYTE[ REG_Cdsp_Skippixel ] = skipPixel & 0x0f;

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspDramVSizeOffsetSet(UINT16 rHSize, UINT16 rHOffset, UINT16 rVSize, UINT16 rVOffset)
 * @brief     Set CDSP Read offset & size from SDRAM.
 * @param     [in] rHSize : Read horizontal size from SDRAM (12 bits)
 * @param     [in] rHOffset : Read horizontal offset from SDRAM (12 bits)
 * @param     [in] rVSize : Read vertical size from SDRAM (12 bits)
 * @param     [in] rVOffset : Read vertical offset from SDRAM (12 bits)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspDramVSizeOffsetSet(
	UINT16 rHSize,
	UINT16 rHOffset,
	UINT16 rVSize,
	UINT16 rVOffset
)
{
	XBYTE[ REG_Cdsp_Rhsize_low ] = LowByte( rHSize );
	XBYTE[ REG_Cdsp_Rhsize_high ] = HighByte( rHSize );

	XBYTE[ REG_Cdsp_Rhoffset_low ] = LowByte( rHOffset );
	XBYTE[ REG_Cdsp_Rhoffset_high ] = HighByte( rHOffset );

	XBYTE[ REG_Cdsp_Rvsize_low ] = LowByte( rVSize );
	XBYTE[ REG_Cdsp_Rvsize_high ] = HighByte( rVSize );

	XBYTE[ REG_Cdsp_Rvoffset_low ] = LowByte( rVOffset );
	XBYTE[ REG_Cdsp_Rvoffset_high ] = HighByte( rVOffset );

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspDramVModeSet(UINT8 remode)
 * @brief     Set source type when reading data from SDRAM
 * @param     [in] remode
 *					 0: RGB 8 bit mode (Bayer)
 *					 1: RGB 10 Bit mode (Bayer)
 *					 2: YUV mode
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/


UINT8
HAL_CdspDramVModeSet(
	UINT8 remode
)
{
	XBYTE[ REG_Cdsp_Remode ] = ( remode & 0x03 ) ;

	return HALK_SUCCESS;
}





/**
 * @fn        UINT8 HAL_CdspVDramSkipWr(UINT8 skip, UINT8 skipSVd)
 * @brief     sync mode for skiping writing data 
 * @param     [in] sync
 *						0 -> Immidate Update
 *						1 -> reflected at VD falling edge 
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_CdspVDramSkipWrSyncSet(
	UINT8 sync
)
{
	XBYTE[ REG_Cdsp_pSkipwr ] &= 0xef; 
	XBYTE[ REG_Cdsp_pSkipwr ] |= (sync&0x01)<<4;

}

/**
 * @fn        void HAL_CdspVDramSkipWrSet(UINT8 val)
 * @brief     Set skip wirte
 * @param     [in] val
 * @param     [in] skipImg :[0] 1: skip image
 *\n						 	0: no skip image
 *\n 					    [1] 1: skip AWB
 *\n						 	0: no skip AWB
 *\n					    [2] 1: skip AE
 *\n						 	0: no skip AE 
 *\n					    [3] 1: skip DIST edge 
 *\n						 	0: no skip DIST edge     
 *\n						[4] 0 -> Immidate Update
 *\n							1 -> reflected at VD falling edge  
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-7-5
 * @todo      N/A
 * @bug       N/A
*/
void 
HAL_CdspVDramSkipWrSet(
	UINT8 mask,
	UINT8 val

)
{
	mask |= 0xf0;
	XBYTE[ REG_Cdsp_pSkipwr ] = (XBYTE[REG_Cdsp_pSkipwr]& mask)|(val&(~(mask)));	
}

/**
 * @fn        void HAL_CdspVDramSkipWrGet(UINT8 *val)
 * @brief     Get register value of skip write
 * @param     [in] val
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-7-5
 * @todo      N/A
 * @bug       N/A
*/
void 
HAL_CdspVDramSkipWrGet(
	UINT8 *val
)
{
	*val = XBYTE[ REG_Cdsp_pSkipwr ];

}

/**
 * @fn        UINT8 HAL_CdspVDramDataType(UINT8 cdspType, UINT8 cdspSync, UINT8 cdspRange)
 * @brief     Config format of data which CDSP write to DRRAM
 * @param     [in] cdspType
 *						0 -> YUV Format
 *						1 -> Raw 8 Bits (From Gamma) 
 *						2 -> Raw 10 Bits(From WB Gain)
 * @param     [in] cdspSync
 *						0 -> Immidate Update
 *						1 -> reflected at VD falling edge
 * @param     [in] cdspRange
 *						0 ->    0 ~ 256
 *						1 -> -128 ~ 127
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_CdspVDramDataType(
	UINT8 cdspType,
	UINT8 cdspSync,
	UINT8 cdspRange
)
{
    XBYTE[REG_Cdsp_imgmode]= ((cdspRange&0x01)<<5) | ((cdspSync&0x01)<<4) | (cdspType&0x03);

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspVDramSizeClamp(UINT8 hSizeClpEn, UINT16 hSize, UINT8 vSizeClpEn, UINT16 vSize, UINT8 sizeSVd)
 * @brief     Set CDSP write  clamp size
 * @param     [in] hSizeClpEn
 *							0 -> Disable
 *							1 -> Enable 
 * @param     [in] hSize : Horizontal clamp size (12 Bits)
 * @param     [in] vSizeClpEn
 *							0 -> Disable
 *							1 -> Enable 
 * @param     [in] vSize : Vertical clamp size (12 Bits)
 * @param     [in] sizeSVd : 
 *							0 -> Immidate Update
 *							1 -> reflected at VD falling edge
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspVDramSizeClamp(
	UINT8 hSizeClpEn,
	UINT16 hSize,
	UINT8 vSizeClpEn,
	UINT16 vSize,
	UINT8 sizeSVd
)
{
	//  UINT16 i;
	XBYTE[ REG_Cdsp_ClampSizeSvd ] = sizeSVd;

	XBYTE[ REG_Cdsp_clamphsizeen ] = hSizeClpEn;
	XBYTE[ REG_Cdsp_Clamphsize_low ] = LowByte( hSize );
	XBYTE[ REG_Cdsp_Clamphsize_high ] = HighByte( hSize );

	XBYTE[ REG_Cdsp_clampvsizeen ] = vSizeClpEn;
	XBYTE[ REG_Cdsp_Clampvsize_low ] = LowByte( vSize );
	XBYTE[ REG_Cdsp_Clampvsize_high ] = HighByte( vSize );

	return HALK_SUCCESS;
}


/**
 * @fn        UINT8 HAL_CdspRawImgSource(UINT8 srcSel)
 * @brief     Set CDSP Image Source
 * @param     [in] srcSel : 0 -> From Front Interface
 *							1 -> From SDRAM
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspRawImgSource(
	UINT8 srcSel
)
{
	XBYTE[ REG_Cdsp_SourceSel ] = srcSel;

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspCoreImgSource(UINT8 srcSel, UINT8 sync)
 * @brief     Set Core Mux Image Source
 * @param     [in] src : 0 -> From Gamma
 *							1 -> From SDRAM
 * @param     [in] sync : 0 -> immediately 
 *						  1 -> sync with VD	
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-8-2
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
HAL_CdspCoreImgSource(
	UINT8 src,
	UINT8 sync
)
{
	XBYTE[ REG_Cdsp_CoreImgSrc ] = (src&0x01) | ((sync&0x01)<<4);
	return HALK_SUCCESS;	
}

/**
 * @fn        UINT8 HAL_CdspEofWait(UINT8 num)
 * @brief     wait for CDSP EOF
 * @param     [in] num : times of waiting for CDSP EOF
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspEofWait(
	UINT8 num
)
{
	UINT8 i;
	UINT8 status;

	ENTER_CRITICAL( status );

	//XBYTE[REG_Cdsp_evnt1] |= 0x01;

	for (i = 0; i< num; i++) {
       	XBYTE[REG_Cdsp_evnt1] |= 0x01;
		while((XBYTE[REG_Cdsp_evnt1] & 0x01) != 0x01) {
        	;//printf("XBYTE[0x21EE]=%bx\n",XBYTE[REG_Cdsp_evnt1]);
		}
	}

	EXIT_CRITICAL( status );

	return 0;
}

/**
 * @fn        UINT8 HAL_CdspDramVInterval(UINT16 lineInterval)
 * @brief     Set interval between line to line
 * @param     [in] lineInterval : Pixels between line to line
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
HAL_CdspDramVInterval
(
    UINT16 lineInterval
)
{
   XBYTE[REG_Cdsp_Lintval_low] =(UINT8)(lineInterval&0xff);
   XBYTE[REG_Cdsp_Lintval_high]=(UINT8)((lineInterval>>8)&0x01);
   
   return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspAEWinEnSet(UINT8 aeWinEn, UINT8 pAeWinHold, UINT8 aeWinTest, UINT8 aeWinMode, UINT8 aeHoldSvdEn, UINT8 aeSizeZf)
 * @brief     Set AE window enable
 * @param     [in] aeWinEn : AE window enable (1 bit)
 * @param     [in] aeWinTest : AE/AWB window position setting test (1 bit) 
 *								0: disable
 *								1: enable 
 * @param     [in] aeHoldSvdEn : Written Aewinhold register value update method (1 bit)
 *								0: immediate update
 *								1: reflected at the next valid VD edge (1 bit)
 * @param     [in] aeSizeZf : AE Window Parameter update method (1 bit)
 *								0: depends on 2240[4]
 *								1: update when zoomfactor update
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspAEWinEnSet(
	UINT8 aeWinEn,
	UINT8 aeHoldSvdEn,
    UINT8 aeSizeZf
)
{
   	XBYTE[ REG_Cdsp_AEwinen ] &= 0xce; 
	XBYTE[ REG_Cdsp_AEwinen ] |= ((aeSizeZf & 0x01)<<5 | ( aeHoldSvdEn & 0x01 ) << 4 | aeWinEn & 0x01);
   
	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspAEWinModeSet(UINT8 aeWinMode)
 * @brief     HAL_CdspAEWinModeSet
 * @param     [in] aeWinMode : AE Window mode (1 bit)
 *								0: Y Window
 *								1: R/Gr/B/Gb Window
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-6-12
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
HAL_CdspAEWinModeSet(
	UINT8 aeWinMode
)
{
	XBYTE[ REG_Cdsp_AEwinen ] &= 0xf7;
	XBYTE[ REG_Cdsp_AEwinen ] |= (aeWinMode&0x01)<<3 ;

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspAEWinModeGet(void)
 * @brief     HAL_CdspAEWinModeGet
 * @param     NONE
 * @retval    return = AE Window mode (1 bit)
 *						0: Y Window
 *						1: R/Gr/B/Gb Window
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-6-12
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
HAL_CdspAEWinModeGet(
	void
)
{
	return ((XBYTE[ REG_Cdsp_AEwinen ] & 0x08)>>3);
}

/**
 * @fn        UINT8 HAL_CdspAEWinHoldSet(UINT8 aeWinHold)
 * @brief     HAL_CdspAEWinHoldSet
 * @param	  [in] pAeWinHold : freeze AE window accumulation value (1 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-6-12
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
HAL_CdspAEWinHoldSet(
	UINT8 aeWinHold	
)
{
	XBYTE[ REG_Cdsp_AEwinen ] &= 0xfd;
	XBYTE[ REG_Cdsp_AEwinen ] |= (aeWinHold&0x01)<<1 ;

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_IntrCdspAEWinHoldSet(UINT8 aeWinHold)
 * @brief     HAL_CdspAEWinHoldSet
 * @param	  [in] pAeWinHold : freeze AE window accumulation value (1 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-6-12
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
HAL_IntrCdspAEWinHoldSet(
	UINT8 aeWinHold	
)
{
	XBYTE[ REG_Cdsp_AEwinen ] &= 0xfd;
	XBYTE[ REG_Cdsp_AEwinen ] |= (aeWinHold&0x01)<<1 ;

	return HALK_SUCCESS;
}


UINT8 
HAL_CdspAEWinTest(
	UINT8 aeWinTest
)
{
	XBYTE[ REG_Cdsp_AEwinen ] &= 0xfb;
	XBYTE[ REG_Cdsp_AEwinen ] |= (aeWinTest&0x01)<<2 ;

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspAEwinfactorset(UINT8 aeHAccFactor, UINT8 aeVAccFactor)
 * @brief     Set AE window accumulator normalizing factor
 * @param     [in] aeHAccFactor : AE window horizontal accumulator normalizing factor (3 bit)
 * @param     [in] aeVAccFactor : AE window vertical accumulator normalizing factor (3 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspAEwinfactorset(
	UINT8 aeHAccFactor,
	UINT8 aeVAccFactor
)
{
	XBYTE[ REG_Cdsp_AEhaccfactor ] = ( aeVAccFactor & 0x07 ) << 4 | ( aeHAccFactor & 0x07 );

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspAEwinoffset(UINT16 aeHOffset, UINT16 aeVOffset)
 * @brief     Set AE window Offset
 * @param     [in] aeHOffset : AE Window Horizontal Offse (10 bit)
 * @param     [in] aeVOffset : AE Window Vertical Offset (9 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspAEwinoffset(
	UINT16 aeHOffset,
	UINT16 aeVOffset
)
{	
	XBYTE[ REG_Cdsp_AEhoffset_low ] = LowByte( aeHOffset );
	XBYTE[ REG_Cdsp_AEhoffset_high ] = HighByte( aeHOffset );

	XBYTE[ REG_Cdsp_AEvoffset_low ] = LowByte( aeVOffset );
	XBYTE[ REG_Cdsp_AEvoffset_high ] = HighByte( aeVOffset );

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspAEwinsizeset(UINT8 aeHSize, UINT8 aeVSize)
 * @brief     Set AE window size
 * @param     [in] aeHSize : AE Window Horizontal Size (8 bit)
 * @param     [in] aeVSize : AE Window Vertical Size (8 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspAEwinsizeset(
	UINT8 aeHSize,
	UINT8 aeVSize
)
{
	XBYTE[ REG_Cdsp_AEhsize ] = aeHSize;
	XBYTE[ REG_Cdsp_AEvsize ] = aeVSize;

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspAEwinintvlset(UINT8 aeHIntvl, UINT8 aeVIntvl)
 * @brief     Set AE Window horizontal/vertical interval between windows
 * @param     [in] aeHIntvl : AE Window horizontal interval between windows (4 bit)
 * @param     [in] aeVIntvl : AE Window vertical interval between windows  (4 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspAEwinintvlset(
	UINT8 aeHIntvl,
	UINT8 aeVIntvl
)
{
	XBYTE[ REG_Cdsp_AEintvl ] = ( aeVIntvl & 0x0f ) << 4 | aeHIntvl & 0x0f;

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspAEwinAddrset(UINT32 aeWinAddr)
 * @brief     Set AE Window Storing address of  AWB window value in DRAM
 * @param     [in] aeWinAddr : Storing address of  AE window value in DRAM (A buffer) (25 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspAEwinAddrset(
	UINT32 aeWinAddr
)
{
	//	printf("HAL_CdspAEwinAddrset AEWinaddr =0x%lx \n",AEWinaddr);
	XBYTE[ REG_Cdsp_AEWinaddr_Byte0 ] = READ8( aeWinAddr, 3 );
	XBYTE[ REG_Cdsp_AEWinaddr_Byte1 ] = READ8( aeWinAddr, 2 );
	XBYTE[ REG_Cdsp_AEWinaddr_Byte2 ] = READ8( aeWinAddr, 1 );
	XBYTE[ REG_Cdsp_AEWinaddr_Byte3 ] = READ8( aeWinAddr, 0 );

	#if 0
		printf("XBYTE[REG_Cdsp_AEWinaddr_Byte0]=0x%bx \n",XBYTE[REG_Cdsp_AEWinaddr_Byte0]);
		printf("XBYTE[REG_Cdsp_AEWinaddr_Byte1]=0x%bx \n",XBYTE[REG_Cdsp_AEWinaddr_Byte1]);
		printf("XBYTE[REG_Cdsp_AEWinaddr_Byte2]=0x%bx \n",XBYTE[REG_Cdsp_AEWinaddr_Byte2]);
		printf("XBYTE[REG_Cdsp_AEWinaddr_Byte3]=0x%bx \n",XBYTE[REG_Cdsp_AEWinaddr_Byte3]);
	#endif

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspAEwinCntset(UINT8 aeHCnt, UINT8 aeVCnt)
 * @brief     Set AE window amounts
 * @param     [in] aeHCnt : AE Window Horizontal amount  (3 bit)
 * @param     [in] aeVCnt :  AE Window Vertical amount (3 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspAEwinCntset(
	UINT8 aeHCnt,
	UINT8 aeVCnt
)
{  
    //UINT8 temp;
	//temp=XBYTE[REG_Cdsp_AEwinen];
	
    if ((XBYTE[REG_Cdsp_AEwinen]&0x08)!= 0){
		ASSERT((aeHCnt<=5) && (aeVCnt<=5),0);
		//if (AEhcnt>5) AEhcnt=5;
		//if (AEvcnt>5) AEvcnt=5;
    }							
		
    XBYTE[ REG_Cdsp_AEwincnt ] = ( (aeVCnt-1) & 0x07 ) << 4 | (aeHCnt-1) & 0x07;

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspAEWinRead(UINT8 aeWinNum, Num,UINT8 *aeWin)
 * @brief     Read the AE Window
 * @param     [in] aeWinNum : the amount of AE window
 * @param     [out] aeWin : Reture 64 windows value
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_CdspAEWinRead(UINT8 aeWinNum,UINT8 *aeWin)
{
	#if 1
		UINT16	i;
		UINT8  lbyte, hbyte;
		UINT32	aeWinAddr;

		/* Front Pattern Set */
		//XBYTE[0x2ae0]=0x80;//white
		
		/*
		AEWinaddr  = (UINT32)(XBYTE[REG_Cdsp_AEWinaddr_Byte0]|
					(XBYTE[REG_Cdsp_AEWinaddr_Byte1] << 8)  |
					(XBYTE[REG_Cdsp_AEWinaddr_Byte2] << 16) |
					(XBYTE[REG_Cdsp_AEWinaddr_Byte3] << 24));
		*/

		
	    aeWinAddr  =  (UINT32)(XBYTE[REG_Cdsp_AEWinaddr_Byte0]);
		aeWinAddr  += (UINT32)(XBYTE[REG_Cdsp_AEWinaddr_Byte1])<<8;
		aeWinAddr  += (UINT32)(XBYTE[REG_Cdsp_AEWinaddr_Byte2])<<16;
		aeWinAddr  += (UINT32)(XBYTE[REG_Cdsp_AEWinaddr_Byte3])<<24;

					 
		//printf("AEWinaddr =0x%lx \n",AEWinaddr);
	    
		
		M_DRAM_SetStartAddress((UINT32)(aeWinAddr), 1);
		for(i=0;i<aeWinNum/2;i++){
			
		    //while(XBYTE[REG_Dram_IndirectPrefetch]);
		    M_DRAM_ReadWord(lbyte, hbyte);
		    aeWin[i*2+1] = hbyte;
		    aeWin[i*2]   = lbyte;
		    
			//printf ("AE Window (%d, %d) = (%bx, %bx) \n", i*2+1, i*2, hbyte, lbyte);
		}
	#else
		//UINT8 xdata* idata pAEWin;
		UINT16 i = 0;
	
		XBYTE[REG_Dram_IndirectAddr_B0] = XBYTE[REG_Cdsp_AEWinaddr_Byte0];//set SDRAM starting addr
		XBYTE[REG_Dram_IndirectAddr_B1] = XBYTE[REG_Cdsp_AEWinaddr_Byte1];
		XBYTE[REG_Dram_IndirectAddr_B2] = XBYTE[REG_Cdsp_AEWinaddr_Byte2];
		XBYTE[REG_Dram_IndirectAddr_B3] = XBYTE[REG_Cdsp_AEWinaddr_Byte3];
	
		//printf("AEWinaddr = 0x%bx%bx%bx%bx\n",XBYTE[REG_Cdsp_AEWinaddr_Byte3],XBYTE[REG_Cdsp_AEWinaddr_Byte2],XBYTE[REG_Cdsp_AEWinaddr_Byte1],XBYTE[REG_Cdsp_AEWinaddr_Byte0]);
		XBYTE[REG_Dram_IndirectPrefetch] = 0x01;
	
		for(i=0;i<=31;i+=1){
		//do {
			while(XBYTE[REG_Dram_IndirectPrefetch]);
			//	*AEWin = XBYTE[REG_Dram_IndirectRData_Low]; //LSB
			//	++AEWin;
			//	*AEWin = XBYTE[REG_Dram_IndirectRData_High]; //MSB
			//	++AEWin;
				AEWin[i*2]	  =XBYTE[REG_Dram_IndirectRData_Low];
				AEWin[i*2+1]=XBYTE[REG_Dram_IndirectRData_High];
	
				//printf ("AE Window (%d, %d) = (%bx, %bx) \n", i*2+1, i*2, AEWin[i*2+1],AEWin[i*2]);
			//} while (--i);
		}
	#endif

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspAWBWinEn(UINT8 awbWinEn, UINT8 pAwbWinHold, UINT8 awbWinTest, UINT8 awbWinHoldSvd, UINT8 awbSizeZfEn)
 * @brief     Set AWB window enable
 * @param     [in] awbWinEn : AWB window enable (1 bit)
 * @param     [in] pAwbWinHold : Freeze AWB window accumulation value (1 bit)
 * @param     [in] awbWinTest : AWB window position setting test
 *								0: disable	
 *								1: enable (1 bit)
 * @param     [in] awbWinHoldSvd : AWB window hold to sync with VD
 *								0: immediate update
 *								1: reflected at the next valid VD edge (1 bit)
 * @param     [in] awbSizeZfEn : AWB window hold to sync with ZF
 *								0: depend on awbWinHoldSvd
 *								1: sync with ZF
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspAWBWinEn(
	UINT8 awbWinEn,
	UINT8 pAwbWinHold,
	UINT8 awbWinTest,
	UINT8 awbWinSvd,
	UINT8 awbSizeZfEn
)
{
	XBYTE[ REG_Cdsp_AWBwinen ] = (awbSizeZfEn & 0x01) <<5 | ( awbWinSvd & 0x01 ) << 4 | ( awbWinTest & 0x01 ) << 2 | ( pAwbWinHold & 0x01 ) << 1 | awbWinEn & 0x01;

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_IntrCdspAWBWinEn(UINT8 awbWinEn, UINT8 pAwbWinHold, UINT8 awbWinTest, UINT8 awbWinHoldSvd, UINT8 awbSizeZfEn)
 * @brief     Set AWB window enable
 * @param     [in] awbWinEn : AWB window enable (1 bit)
 * @param     [in] pAwbWinHold : Freeze AWB window accumulation value (1 bit)
 * @param     [in] awbWinTest : AWB window position setting test
 *								0: disable	
 *								1: enable (1 bit)
 * @param     [in] awbWinHoldSvd : AWB window hold to sync with VD
 *								0: immediate update
 *								1: reflected at the next valid VD edge (1 bit)
 * @param     [in] awbSizeZfEn : AWB window hold to sync with ZF
 *								0: depend on awbWinHoldSvd
 *								1: sync with ZF
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_IntrCdspAWBWinEn(
	UINT8 awbWinEn,
	UINT8 pAwbWinHold,
	UINT8 awbWinTest,
	UINT8 awbWinSvd,
	UINT8 awbSizeZfEn
)
{
	XBYTE[ REG_Cdsp_AWBwinen ] = (awbSizeZfEn & 0x01) <<5 | ( awbWinSvd & 0x01 ) << 4 | ( awbWinTest & 0x01 ) << 2 | ( pAwbWinHold & 0x01 ) << 1 | awbWinEn & 0x01;

	return HALK_SUCCESS;
}


/**
 * @fn        UINT8 HAL_CdspAWBWinScaleEn(UINT8 awbHScaleEn, UINT8 awbVScaleEn)
 * @brief     HAL_CdspAWBWinScaleEn
 * @param     [in] awbHScaleEn : AWB Horizontal Scaling Enable (1 bit)
 * @param     [in] awbVScaleEn : AWB Vertical  Scaling Enable (1 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspAWBWinScaleEn(
	UINT8 awbHScaleEn,
	UINT8 awbVScaleEn
)
{
	XBYTE[ REG_Cdsp_AWBscaleen ] = ( awbVScaleEn & 0x01 ) << 4 | awbHScaleEn & 0x01;

	return HALK_SUCCESS;
}


/**
 * @fn        UINT8 HAL_CdspAWBWincfg(UINT16 awbHStart, UINT16 awbHEnd, UINT16 awbVStart, UINT16 awbVEnd, UINT16 awbHScaleFactor, UINT16 awbVScaleFactor)
 * @brief     Set AWB window AWB Window horizontal/vertical  Star/end position
 * @param     [in] awbHStart : AWB Horizontal Window Start (12 bits)
 * @param     [in] awbHEnd : AWB Vertical Window Start (12 bits)
 * @param     [in] awbVStart : AWB Vertical Window Start (12 bits)
 * @param     [in] awbVEnd : AWB Horizontal Window Start (12 bits)
 * @param     [in] awbHScaleFactor : AWB Sub-Sample H Factor(16 bit)
 * @param     [in] awbVScaleFactor : AWB Sub-Sample V Factor(16 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspAWBWincfg(
	UINT16 awbHStart,
	UINT16 awbHEnd,
	UINT16 awbVStart,
	UINT16 awbVEnd,
	UINT16 awbHScaleFactor,
	UINT16 awbVScaleFactor
)
{
	XBYTE[ REG_Cdsp_AWBhstar_low ] = LowByte( awbHStart );
	XBYTE[ REG_Cdsp_AWBhstar_high ] = HighByte( awbHStart );

	XBYTE[ REG_Cdsp_AWBhend_low ] = LowByte( awbHEnd );
	XBYTE[ REG_Cdsp_AWBhend_high ] = HighByte( awbHEnd );

	XBYTE[ REG_Cdsp_AWBvstart_low ] = LowByte( awbVStart );
	XBYTE[ REG_Cdsp_AWBvstart_high ] = HighByte( awbVStart );

	XBYTE[ REG_Cdsp_AWBvend_low ] = LowByte( awbVEnd);
	XBYTE[ REG_Cdsp_AWBvend_high ] = HighByte( awbVEnd );

	XBYTE[ REG_Cdsp_AWBhscalefactor_low ] = LowByte( awbHScaleFactor );
	XBYTE[ REG_Cdsp_AWBhscalefactor_high ] = HighByte( awbHScaleFactor );

	XBYTE[ REG_Cdsp_AWBvscalefactor_low ] = LowByte( awbVScaleFactor );
	XBYTE[ REG_Cdsp_AWBvscalefactor_high ] = HighByte( awbVScaleFactor );

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspAWBwinAddrset(UINT32 awbWinAddr)
 * @brief     Set AWB Window Storing address of  AWB window value in DRAM *
 * @param     [in] awbWinAddr : Storing address of  AWB window value
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspAWBwinAddrset(
	UINT32 awbWinAddr
)
{
	//	printf("AWBWinaddr =%x \n",AWBWinaddr);
	XBYTE[ REG_Cdsp_AWBWinaddr_Byte0 ] = READ8( awbWinAddr, 3 );
	XBYTE[ REG_Cdsp_AWBWinaddr_Byte1 ] = READ8( awbWinAddr, 2 );
	XBYTE[ REG_Cdsp_AWBWinaddr_Byte2 ] = READ8( awbWinAddr, 1 );
	XBYTE[ REG_Cdsp_AWBWinaddr_Byte3 ] = READ8( awbWinAddr, 0 );
	/*printf("XBYTE[0x2230] =%x \n",XBYTE[0x2230]);
	printf("XBYTE[0x2231] =%x \n",XBYTE[0x2231]);
	printf("XBYTE[0x2232] =%x \n",XBYTE[0x2232]);
	printf("XBYTE[0x2233] =%x \n",XBYTE[0x2233]);*/

	return HALK_SUCCESS;
}


/**
 * @fn        UINT8 HAL_CdspAWBWindowSizeSet(UINT16 awbHStart, UINT16 awbHEnd, UINT16 awbVStart, UINT16 awbVEnd, UINT8 winHSize, UINT8 winVSize)
 * @brief     Set AWB Window Size
 * @param     [in] awbHStart : Horizontal start positon 
 * @param     [in] awbHEnd : Horizontal end positon
 * @param     [in] awbVStart : Vertical start positon 
 * @param     [in] awbVEnd : Vertical end positon 
 * @param     [in] winHSize : Horizontal window size
 * @param     [in] winVSize : Vertical window size
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspAWBWindowSizeSet(
	UINT16 awbHStart,
	UINT16 awbHEnd,
	UINT16 awbVStart,
	UINT16 awbVEnd,
	UINT8 winHSize,
	UINT8 winVSize
)
{
	UINT16 hSize, vSize, hScaleFactor, vScaleFactor;
	//UINT16 i;
	HAL_CdspAWBWinScaleEn( 0, 0 );

	if( awbVEnd <= awbVStart ) {
		return HALK_ERROR_PARAMETER;
	}
	vSize = awbVEnd - awbVStart +1;
	vScaleFactor = ( UINT16 ) ( ( ( UINT32 ) winVSize << 16 ) / vSize ) + 1;

	if( awbHEnd <= awbHStart ) {
		return HALK_ERROR_PARAMETER;
	}
	
	hSize = awbHEnd - awbHStart +1;
	hScaleFactor = ( UINT16 ) ( ( ( UINT32 ) winHSize << 16 ) / hSize ) + 1;


	HAL_CdspAWBWincfg( awbHStart, awbHEnd, awbVStart, awbVEnd, hScaleFactor, vScaleFactor );
	HAL_CdspAWBWinScaleEn( 1, 1 );
	// HAL_CdspAWBWinEn( 0x01, 0x0, 0x00, 0x1 ,0 ); 
	//     printf("width=0x%x,high=0x%x\n",width,high);
	//printf("hsize=%d,vsize=%d\n",hSize,vSize);
	//printf("hscalefactor=0x%x,vscalefactor=0x%x\n",hScaleFactor,vScaleFactor);
	
}


/**
 * @fn        UINT8 HAL_CdspAWBGainSet(UINT8 awbRGain, ,UINT8 awbBGain)
 * @brief     Set R/G gain
 * @param     [in] awbRGain : R Gain (2.6 bits)
 * @param     [in] awbBGain : B Gain (2.6 bits)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
HAL_CdspAWBWndGainSet(
	UINT8 awbRGain,
	UINT8 awbBGain
)
{
    XBYTE[REG_Cdsp_AWBWinrgain]=awbRGain;
	XBYTE[REG_Cdsp_AWBWinbgain]=awbBGain;
	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspAWBWinRead(UINT16 awbWinNum, UINT8 *awbWin)
 * @brief     Read AWB window value
 * @param     [in] awbWinNum : the amount of AE window
 * @param     [out] awbWin : Return  windows value
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
HAL_CdspAWBWinRead(
	UINT16 awbWinNum,
	UINT8 *awbWin
)
{
	UINT16	i;
	UINT8  lbyte, hbyte;
	UINT32	awbWinAddr;

	awbWinAddr  =  (UINT32)(XBYTE[REG_Cdsp_AWBWinaddr_Byte0]);
	awbWinAddr  += (UINT32)(XBYTE[REG_Cdsp_AWBWinaddr_Byte1])<<8;
	awbWinAddr  += (UINT32)(XBYTE[REG_Cdsp_AWBWinaddr_Byte2])<<16;
	awbWinAddr  += (UINT32)(XBYTE[REG_Cdsp_AWBWinaddr_Byte3])<<24;
	
				 
	//printf("AWBWinaddr =0x%lx \n",awbWinAddr);
	
	M_DRAM_SetStartAddress((UINT32)(awbWinAddr), 1);	
	
	for(i=0;i<awbWinNum/2;i++){
		//HAL_DRAMindirectread(AEWinaddr+i, &yval);

		while(XBYTE[REG_Dram_IndirectPrefetch]);
		M_DRAM_ReadWord(lbyte, hbyte);
		awbWin[i*2+1] = hbyte;
		awbWin[i*2]	 = lbyte;
		
		//printf ("AE Window (%d, %d) = (%bx, %bx) \n", i*2+1, i*2, hbyte, lbyte);
	}
    
    return HALK_SUCCESS;
}



#ifdef HAL_CDSP_AF_FUNCTION
/**
 * @fn        UINT8 HAL_CdspAFWinEn(UINT8 afwinen, UINT8 pAfWinHold, UINT8 afWinTest, UINT8 afFlt2Type, UINT8 afWinHoldSvd)
 * @brief     Set AF window enable
 * @param     [in] afwinen : AF window enable (1 bit)
 * @param     [in] pAfWinHold : Freeze AF window accumulation value (1 bit)
 * @param     [in] afWinTest : AF window position setting test
 *								0: disable
 *								1: enable (1 bit)
 * @param     [in] afFlt2Type :  Filter type 0 : (0, -1, 2, -1, 0) 1 : (1, -4, 6, -4, 1)
 * @param     [in] afWinHoldSvd : AF window hold to sync with VD (1 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspAFWinEn(
	UINT8 afwinen,
	UINT8 pAfWinHold,
	UINT8 afWinTest,
	UINT8 afFlt2Type,
	UINT8 afWinHoldSvd
)
{
	XBYTE[ REG_Cdsp_AFwinen ] = ( afWinHoldSvd & 0x01 ) << 4 | ( afFlt2Type & 0x01 ) << 3 | ( afWinTest & 0x01 ) << 2 | ( pAfWinHold & 0x01 ) << 1 | afwinen & 0x01;

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspAFdataModeSet(UINT8 afRgbMode, UINT8 afPgType)
 * @brief     Set AF statistic data mode
 * @param     [in] afRgbMode : AF statistic data mode 0 : R   ,1 : G  , 2 : B  ,   3 : R+G+B (1 bit)
 * @param     [in] afPgType : curve type (1 bit)
 *								0 : slop 1/4
 *								1 : slop 1/2
 *								2 : custom gamma1 curve
 *								3 : custom gamma2 curve
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspAFDataModeSet(
	UINT8 afRgbMode,
	UINT8 afPgType
)
{
	XBYTE[ REG_Cdsp_AFrgbmode ] = ( afPgType & 0x03 ) << 4 | afRgbMode & 0x03;

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspAFFilterWtSet(UINT8 afFlt1Wt, UINT8 afFlt2Wt)
 * @brief     Set AF filter weight function
 * @param     [in] afFlt1Wt : AF filter 1 weight = afflt1wt/8 (1.3 bits) (max =1)
 * @param     [in] afFlt2Wt : AF filter 2 weight = afflt2wt/8 (1.3 bits) (max =1)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspAFFilterWtSet(
	UINT8 afFlt1Wt,
	UINT8 afFlt2Wt
)
{
	XBYTE[ REG_Cdsp_AFfilterwt ] = ( afFlt2Wt & 0x0f ) << 4 | ( afFlt1Wt & 0x0f );

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspAFCoringSet(UINT8 afCorThr, UINT8 afCorVal)
 * @brief     Set AF filter coring parameter  function
 * @param     [in] afCorThr : AF coring threshold (8 bit)
 * @param     [in] afCorVal : AF coring value (8 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspAFCoringSet(
	UINT8 afCorThr,
	UINT8 afCorVal
)
{ 
	//ASSERT(afcorval<=afcorthr,0);
	if (afCorVal>afCorThr){
		return HALK_ERROR_PARAMETER;
	}
		
	
	XBYTE[ REG_Cdsp_AFcoringthr ] = afCorThr;
	XBYTE[ REG_Cdsp_AFcoringvalue ] = afCorVal;

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspAFWinSizeSet(UINT16 afHStart, UINT16 afHEnd, UINT16 afVStart, UINT16 afVEnd)
 * @brief     Set AF window size function
 * @param     [in] afHStart : AF window horizontal start (12 bit)
 * @param     [in] afHEnd : AF window horizontal end (12 bit)
 * @param     [in] afVStart : AF window vertical start (12 bit)
 * @param     [in] afVEnd : AF window vertical end (12 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspAFWinSizeSet(
	UINT16 afHStart,
	UINT16 afHEnd,
	UINT16 afVStart,
	UINT16 afVEnd
)
{
	XBYTE[ REG_Cdsp_AFhstart_low ] = LowByte( afHStart );
	XBYTE[ REG_Cdsp_AFhstart_high ] = HighByte( afHStart );

	XBYTE[ REG_Cdsp_AFhend_low ] = LowByte( afHEnd );
	XBYTE[ REG_Cdsp_AFhend_high ] = HighByte( afHEnd );

	XBYTE[ REG_Cdsp_AFvstart_low ] = LowByte( afVStart );
	XBYTE[ REG_Cdsp_AFvstart_high ] = HighByte( afVStart );

	XBYTE[ REG_Cdsp_AFvend_low ] = LowByte( afVEnd );
	XBYTE[ REG_Cdsp_AFvend_high ] = HighByte( afVEnd );

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_ReadAFWindow(UINT32* afWinValue)
 * @brief     Read the AF Window
 * @param     [out] afWinValue : Reture AF Value (unsigned long  value)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_ReadAFWindow(
	UINT32* afWinValue
)
{
	UINT32 lValue, hValue, value;
	*afWinValue = 0;
	
	XBYTE[REG_Cdsp_evnt1]|=0x10;
	while ((XBYTE[REG_Cdsp_evnt1]&0x10)==0x00) {;}   //will: wait for AF Done	
	HAL_FrontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
	#if 0
		printf("-----XBYTE[REG_Cdsp_AFvalue_byte0] = 0x%bx \n",XBYTE[REG_Cdsp_AFvalue_byte0] );
		printf("-----XBYTE[REG_Cdsp_AFvalue_byte1] = 0x%bx \n",XBYTE[REG_Cdsp_AFvalue_byte1] );
		printf("-----XBYTE[REG_Cdsp_AFvalue_byte2] = 0x%bx \n",XBYTE[REG_Cdsp_AFvalue_byte2] );
		printf("-----XBYTE[REG_Cdsp_AFvalue_byte3] = 0x%bx \n",XBYTE[REG_Cdsp_AFvalue_byte3] );
	#endif

	lValue = ( UINT8 ) XBYTE[ REG_Cdsp_AFvalue_byte0 ];
	lValue = lValue + ( UINT16 ) ( XBYTE[ REG_Cdsp_AFvalue_byte1 ] << 8 );
	hValue = ( UINT8 ) XBYTE[ REG_Cdsp_AFvalue_byte2 ] ;
	hValue = hValue + ( UINT16 ) ( XBYTE[ REG_Cdsp_AFvalue_byte3 ] << 8 );
	value = ( UINT32 ) ( hValue << 16 ) + ( UINT32 ) lValue;
	//     printf("-----value = 0x%lx\n",value );
	*afWinValue = value;
	//	 printf("-----*AFWinvalue = 0x%lx\n",*AFWinvalue );

	return HALK_SUCCESS;
}
#endif


/**
 * @fn        UINT8 HAL_CdspDistEdgeWinEn(UINT8 edgeWinEn, UINT8 edgeWinHold, UINT8 edgeWinTest, UINT8 edgePosition, UINT8 edgeHoldSvdEn)
 * @brief     Set EDGE window enable
 * @param     [in] edgeWinEn : EDGE window enable (1 bit)
 * @param     [in] edgeWinHold : Freeze Edge window accumulation value (1 bit)
 * @param     [in] edgeWinTest : Edge window position setting test (1 bit)
 *									0: disable				   
 *									1: enable 
 * @param     [in] edgePosition : 
 *									0: After YUV second path
 *									1: After YUV first path (h size <= 320) 
 * @param     [in] edgeHoldSvdEn :
 *									0: immediate update
 *									1: update at the next VD
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspDistEdgeWinEn(
	UINT8 edgeWinEn,
	UINT8 edgeWinHold,
	UINT8 edgeWinTest,
    UINT8 edgePosition,	
    UINT8 edgeHoldSvdEn
)
{
   
	XBYTE[ REG_Cdsp_Edgewinen ] = (edgeWinTest & 0x01)<<7 | ( edgeHoldSvdEn & 0x01 ) << 4 | ( edgePosition & 0x01 ) << 2 | ( edgeWinHold & 0x01 ) << 1 | edgeWinEn & 0x01;
   
	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspDistEdgeWinOffSet(UINT8 edgeHOffSet, UINT8 edgeVOffSet)
 * @brief     Set Edge window Offset
 * @param     [in] edgeHOffSet : horizontal offset (8 bits)
 * @param     [in] edgeVOffSet : vertical offset (8 bits)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspDistEdgeWinOffSet(
	UINT8 edgeHOffSet,
	UINT8 edgeVOffSet
)
{
	XBYTE[ REG_Cdsp_Edgehoffset ] = edgeHOffSet;
	XBYTE[ REG_Cdsp_Edgevoffset ] = edgeVOffSet;

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspDistEdgeWinSizeSet(UINT8 edgeHSize, UINT8 edgeVSize)
 * @brief     Set window size
 * @param     [in] edgeHSize : Edge Window Horizontal Size (8 bit)
 * @param     [in] edgeVSize : Edge Window Vertical Size (8 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspDistEdgeWinSizeSet(
	UINT8 edgeHSize,
	UINT8 edgeVSize
)
{
	XBYTE[ REG_Cdsp_Edgehsize ] = edgeHSize;
	XBYTE[ REG_Cdsp_Edgevsize ] = edgeVSize;

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspDistEdgeWinThrSet(UINT8 edgeHThr, edgeHThr,UINT8 edgeVThr)
 * @brief     Set Edge Window horizontal/vertical threshold
 * @param     [in] edgeHThr : Edge Window horizontal threshold (8 bit)
 * @param     [in] edgeVThr : Edge Window vertical threshold (8 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_CdspDistEdgeWinThrSet(
	UINT8 edgeHThr,
	UINT8 edgeVThr
)
{
	XBYTE[ REG_Cdsp_Edgewinthrh ] = edgeHThr;    
	XBYTE[ REG_Cdsp_Edgewinthrv ] = edgeVThr;
	
	return HALK_SUCCESS;	
}

/**
 * @fn        UINT8 HAL_CdspDistEdgeWinAddrSet(UINT32 edgeWinAddrA, UINT32 edgeWinAddrB)
 * @brief     set edge window address in DRAM
 * @param     [in] edgeWinAddrA : Store address of  Edge window value in DRAM (A buffer) (25 bit)
 * @param     [in] edgeWinAddrB : Store address of  Edge window value in DRAM (A buffer) (25 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_CdspDistEdgeWinAddrSet(
	UINT32 edgeWinAddrA,
	UINT32 edgeWinAddrB
)
{
	
	//printf("Edge WinaddrA =%lx \n",edgeWinAddrA);
    //printf("Edge WinaddrB =%lx \n",edgeWinAddrB);
    
	XBYTE[ REG_Dram_EdgeAddr0_B0 ] = READ8( edgeWinAddrA, 3 );
	XBYTE[ REG_Dram_EdgeAddr0_B1 ] = READ8( edgeWinAddrA, 2 );
	XBYTE[ REG_Dram_EdgeAddr0_B2 ] = READ8( edgeWinAddrA, 1 );
	XBYTE[ REG_Dram_EdgeAddr0_B3 ] = READ8( edgeWinAddrA, 0 );

	XBYTE[ REG_Dram_EdgeAddr1_B0 ] = READ8( edgeWinAddrB, 3 );
	XBYTE[ REG_Dram_EdgeAddr1_B1 ] = READ8( edgeWinAddrB, 2 );
	XBYTE[ REG_Dram_EdgeAddr1_B2 ] = READ8( edgeWinAddrB, 1 );
	XBYTE[ REG_Dram_EdgeAddr1_B3 ] = READ8( edgeWinAddrB, 0 );

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspDistEdgeWinCntSet(UINT8 edgeHCnt, t,UINT8 edgeVCnt)
 * @brief     Set Edge window amount
 * @param     [in] edgeHCnt : Edge Window Horizontal amount 
 * @param     [in] edgeVCnt : Edge Window Veritical amount 
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_CdspDistEdgeWinCntSet(UINT8 edgeHCnt,UINT8 edgeVCnt)
{

	//ASSERT((edgeHCnt<=12) && (edgeVCnt<=12),0);	

	if ((edgeHCnt>12) || (edgeVCnt>12)){
		return HALK_ERROR_PARAMETER;
	}
		
    XBYTE[ REG_Cdsp_Edgehwincnt ] = ( (edgeVCnt-1) & 0x0f ) << 4 | (edgeHCnt-1) & 0x0f;

	return HALK_SUCCESS;

}

/**
 * @fn        UINT8 HAL_CdspDistEdgeWinMap(UINT8 edgeHMap, edgeHMap,UINT8 edgeVMap)
 * @brief     Set H/V mapping factor of coordinate, from second path to first path
 * @param     [in] edgeHMap : Horiztontal mapping factor
 * @param     [in] edgeVMap : Vertical mapping factor
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_CdspDistEdgeWinMap(
	UINT8 edgeHMap,
	UINT8 edgeVMap
)
{
							
    XBYTE[ REG_Cdsp_Edgewinhmap ] = edgeHMap;
	XBYTE[ REG_Cdsp_Edgewinvmap ] = edgeVMap;

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspDistEdgeWinRead(UINT16 edgeWinNum, UINT8 *edgeWin)
 * @brief     read edge window value
 * @param     [in] edgeWinNum : edge winodw amount
 * @param     [out] edgeWin : edge window value
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_CdspDistEdgeWinRead(
	UINT16 edgeWinNum,
	UINT8 *edgeWin
)
{
	UINT16	i;
	UINT8  lbyte, hbyte;
	UINT32	edgeWinAddr;

    //if (XBYTE[REG_Dram_MpgSb_CdspWRegIdx]==0

	edgeWinAddr  =  (UINT32)(XBYTE[REG_Dram_EdgeAddr0_B0]);
	edgeWinAddr  += (UINT32)(XBYTE[REG_Dram_EdgeAddr0_B1])<<8;
	edgeWinAddr  += (UINT32)(XBYTE[REG_Dram_EdgeAddr0_B2])<<16;
	edgeWinAddr  += (UINT32)(XBYTE[REG_Dram_EdgeAddr0_B3])<<24;
	
				 
	printf("EDGEWinaddr =0x%lx \n",edgeWinAddr);
	
	M_DRAM_SetStartAddress((UINT32)(edgeWinAddr), 1);	
	
	for(i=0;i<edgeWinNum/2;i++){
		//HAL_DRAMindirectread(AEWinaddr+i, &yval);

		while(XBYTE[REG_Dram_IndirectPrefetch]){;			
		}
		
		M_DRAM_ReadWord(lbyte, hbyte);
		edgeWin[i*2+1] = hbyte;
		edgeWin[i*2]   = lbyte;
		
		//printf ("AE Window (%d, %d) = (%bx, %bx) \n", i*2+1, i*2, hbyte, lbyte);
	}
    
	return HALK_SUCCESS;
}


/**
 * @fn        UINT8 HAL_CdspIntplmir(UINT8 intplMirHLEn, UINT8 intplMirHREn, UINT8 intplMirVTEn, UINT8 intplMirVDEn)
 * @brief     set intpl mirror function
 * @param     [in] intplMirHLEn : Interpolation horizontal left mirror enable (1 bit)
 * @param     [in] intplMirHREn : Interpolation horizontal right mirror enable (1 bit)
 * @param     [in] intplMirVTEn : Interpolation vertical top mirror enable (1 bit)
 * @param     [in] intplMirVDEn : Interpolation vertical down mirror enable (1 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/

UINT8
HAL_CdspIntplmir(
	UINT8 intplMirHLEn,
	UINT8 intplMirHREn,
	UINT8 intplMirVTEn,
	UINT8 intplMirVDEn
)
{
	XBYTE[ REG_Cdsp_intplmirror ] =  ( ( intplMirVDEn & 0x01 ) << 3 ) | ( ( intplMirVTEn & 0x01 ) << 2 ) | ( ( intplMirHREn & 0x01 ) << 1 ) | ( intplMirHLEn & 0x01 );

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspIntplLowThrSet(UINT8 intplLowThr)
 * @brief     Set interpolation de-noise low threshold
 * @param     [in] intplLowThr : Interpolation de-noise low threshold (8 bits)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-10-6
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
HAL_CdspIntplLowThrSet(
	UINT8 intplLowThr
)
{
	XBYTE[REG_Cdsp_intpllowthr]=intplLowThr;
	return HALK_SUCCESS;
}



/**
 * @fn        void HAL_CdspRegBackup(UINT32 addr, UINT16 size, UINT16 *cnt)
 * @brief     Backup CDSP register into dram
 * @param     [in] addr : dram address
 * @param     [in] size : accumulated size
 * @param     [in] cnt : accumulated counter
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_CdspRegBackup(
	void
)
{
	UINT16 i;
	//snapCdspRegCnt=0;
	if( snapCdspRegCnt < snapCdspRegSize ) {
		HAL_DramStartAddrSet( snapCdspRegAddr + ( snapCdspRegCnt >> 1 ), 0 );
	
		for( i = 0; i < 0x200; i = i + 2 ) {
			HAL_DramWordDataWrite( XBYTE[ REG_Cdsp_macroSel + i ], XBYTE[ REG_Cdsp_macroSel + i + 1 ] );
		}

		snapCdspRegCnt += 0x200;
	}


}

void 
HAL_CdspRegBackupBufSet(
	UINT32 addr,
	UINT16 size
)
{
	snapCdspRegAddr=addr;
	snapCdspRegSize=size;
}

void 
HAL_CdspRegBackupCntSet(
	UINT16 cnt
)
{
	snapCdspRegCnt=cnt;	
}


void
HAL_CdspRegGet(
	UINT16 addr,
	UINT8 *val
)
{
	*val = XBYTE[addr];
}

void
HAL_CdspRegSet(
	UINT16 addr,
	UINT8 val
)
{
	XBYTE[addr] = val;
}



/**************************************************************************
 *                     BACKUP                                             *
 **************************************************************************/
#ifndef HAL_CDSP_REMOVE_UNCALL_FUNCTION

/**
 * @fn        UINT8 HAL_CarryWork(UINT32 *tmpvini, UINT16 *vrealdo, UINT16 dovsize, UINT16 vfactor)
 * @brief     HAL_CarryWork
 * @param     [in] tmpvini
 * @param     [in] vrealdo
 * @param     [in] dovsize
 * @param     [in] vfactor
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/

UINT8
HAL_CarryWork(
	UINT32 *tmpvini,
	UINT16 *vrealdo,
	UINT16 dovsize,
	UINT16 vfactor
)
{
	UINT32 tmp, mod;

	tmp = ( ( UINT32 ) dovsize << 16 ) - *tmpvini;
	mod = tmp % ( ( UINT32 ) vfactor );
	
	*vrealdo = (UINT16)( tmp / ( ( UINT32 ) vfactor ) );

	if ( mod != 0 ) {
		(*vrealdo)++;
	}
	
	
	// FIX ME
	*tmpvini += ( ( UINT32 )*vrealdo * ( ( UINT32 ) vfactor ) ) - ( ( ( UINT32 ) dovsize ) << 16 );
	
	return 0;
}

 /**************************************************************************************
 * *
 * Function Name :HAL_Yuv422To444 *
 * *
 * Purposes : *
 * YUV input 422 to 444 enable *
 * *
 * Desprictions : *
 * YUV input 422 to 444 enable *
 * *
 * Arguments : *
 * yuvinserten : in, YUV input 422 to 444 enable (1 bit)*
 * *
 * Returns : *
 * None *
 * *
 * See also : *
 * *
 ***************************************************************************************/

 UINT8
 HAL_Yuv422To444(
	 UINT8 yuvinserten
 
 )
 {
 XBYTE[REG_Cdsp_yuvinserten] = yuvinserten & 0x01;
 return HALK_SUCCESS;
 }

/**
 * @fn        UINT8 HAL_CdspIntplCfg(UINT8 intplLowThr, UINT8 IntplEdgeTthr, UINT8 intplSlntThr, UINT8 intplFtrOpt)
 * @brief     configure interpolation parameters
 * @param     [in] intplLowThr : Interpolation de-noise low threshold (8 bits)
 * @param     [in] IntplEdgeTthr : Interpolation de-noise edge threshold(8 bits)
 * @param     [in] intplSlntThr : Interpolation slant threshold(8 bits)
 * @param     [in] intplFtrOpt : Interpolation de-noise mode (1 bit)
 *									0: light
 *									1: heavy 
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspIntplCfg(
	UINT8 intplLowThr,
	UINT8 IntplEdgeTthr,
	UINT8 intplSlntThr,
	UINT8 intplFtrOpt
)
{
	XBYTE[ REG_Cdsp_intpllowthr ] = intplLowThr;
	XBYTE[ REG_Cdsp_Intpledgthr ] = IntplEdgeTthr;
	XBYTE[ REG_Cdsp_intplslntthr ] = intplSlntThr;
	XBYTE[ REG_Cdsp_intplftropt ] = intplFtrOpt;

	return HALK_SUCCESS;
}



/**************************************************************************************
* *
* Function Name :HAL_CdspRBClampSet *
* *
* Purposes : *
* Set RB clamp value *
* *
* Desprictions : *
* To set RB clamp value *
* *
* Arguments : *
* enable : in, RB clamp enable *
* value : in, RB clamp value (0~255) (8 bit)*
* *
* Returns : *
* None *
* *
* See also : *
* *
***************************************************************************************/

UINT8
HAL_CdspRBClampSet(
	UINT8 enable,
	UINT8 RBClampvalue
)
{
	XBYTE[ REG_Cdsp_rbclamp ] = RBClampvalue;
	XBYTE[ REG_Cdsp_rbclampen ] = enable & 0x01;

	return HALK_SUCCESS;
}


/**************************************************************************************
* *
* Function Name : HAL_CDSPPixSel *
* *
* Purposes : *
* Set Bayer pattern sequence function. *
* *
* Desprictions : *
* Set Bayer pattern sequence function *
* *
* Arguments : *
* pixsw : in, Pixel switch for input image type
* linesw : in, Line switch for input image type*
* *
* Returns : *
* None *
* *
* See also : *
* *
***************************************************************************************/

UINT8
HAL_CDSPPixSel(
	UINT8 pixsw,
	UINT8 linesw
)
{
	XBYTE[ REG_Cdsp_pixsel ] = ( linesw & 0x0f ) << 1 | ( pixsw & 0x01 );

	return HALK_SUCCESS;
}


/**************************************************************************************
* *
* Function Name : HAL_SetWBGlobalGainSet *
* *
* Purposes : *
* Set the Global Gain of raw data. *
* *
* Desprictions : *
* Set the Global Gain of raw data.  *
* *
* Arguments : *
*  GGain :  in, Global Gain of raw data. (3.5 bit)
* *
* Returns : *
* None *
* *
* See also : *
* *
***************************************************************************************/

UINT8
HAL_SetWBGlobalGainSet(
	UINT8 GGain
)
{
	XBYTE[ REG_Cdsp_pdigialglogain ] = GGain;
	//  	  XBYTE[0x215C] = GGain;
	//  	  printf (" Register GGain = %02bx \n", GGain);

	return HALK_SUCCESS;
}

/**************************************************************************************
* *
* Function Name : HAL_LenscmpInit *
* *
* Purposes : *
* Initialize lens compensation table *
* *
* Desprictions : *
* To initiate the lens compensation LUT *
* *
* Arguments : *
* plensdata : in, pointer, data array for lens compensation look up table (total 256 * 16 bit,table 2.8 bit)*
* *
* Returns : *
* None *
* *
* See also : *
* *
***************************************************************************************/

UINT8
HAL_LenscmpInit(
	UINT16* plensdata
)
{
	UINT16 i;
	UINT8 LCh, LCl;
	XBYTE[ REG_Cdsp_macroSel ] = 0x01;
	XBYTE[ REG_Cdsp_switchclk ] = 0x01;
	XBYTE[ REG_Cdsp_macropagsel ] = 0x00;

	/* for (i=0;i<=0xff;i++)
	 {
		XBYTE[0x2b00+i] = (plensdata[i]& 0xff);
	 }
	 XBYTE[REG_Cdsp_macropagsel]= 0x01;
	 for (i=0;i<=0xFF;i++)
	 {
		XBYTE[0x2b00+i] = (plensdata[i]& 0x0300)>>8;
	 }*/

	for( i = 0; i <= 0xff; i++ ) {
		XBYTE[ REG_Cdsp_macropagsel ] = 0x00;
		LCl = ( ( UINT8 ) plensdata[ i ] ) & 0xff;
		XBYTE[ 0x2b00 + i ] = LCl;
		// printf("plensdata[%d] =0x%x \n",i,plensdata[i]);
		// printf(" lplensdata[%d] =0x%bx \n",i,LCl);

		//while ( XBYTE[0x2b00+i] != LCl)
		// {
		XBYTE[ 0x2b00 + i ] = LCl;
		//}

		XBYTE[ REG_Cdsp_macropagsel ] = 0x01;
		LCh = ( UINT8 ) ( ( plensdata[ i ] ) >> 8 );
		XBYTE[ 0x2b00 + i ] = LCh;
		// printf(" hlensdata[%d] =0x%bx \n",i,LCh);
		// while ( XBYTE[0x2b00+i] != LCh)
		// {
		XBYTE[ 0x2b00 + i ] = LCh;
		// }
	}

	/*   for (i=0;i<=0xff;i++)
	   {
		  XBYTE[REG_Cdsp_macropagsel]= 0x00;
		  XBYTE[0x2b00+i] = (i*4) & 0xFF;
		  XBYTE[REG_Cdsp_macropagsel]= 0x01;
		  XBYTE[0x2b00+i] = ((i*4) >> 8) & 0xFF;
	   }*/

	XBYTE[ REG_Cdsp_macroSel ] = 0x00;
	XBYTE[ REG_Cdsp_switchclk ] = 0x00;
	XBYTE[ REG_Cdsp_macropagsel ] = 0x00;
	//*plensdata =0;

	return HALK_SUCCESS;
}

/**************************************************************************************
* *
* Function Name : HAL_CdspCropGet *
* *
* Purposes : *
* To Get the Crop offset for CDSP. *
* *
* Desprictions : *
* *
* Arguments : *
* crop_henable: in, Enable horizontal image cropping (1 bit)
* crop_venable: in,Enable vertical image cropping (1 bit)
*
* crop_hoffset : in, the crop hoffset (12 bit)*
* crop_hsize : in, the crop voffset (12 bit)*
* crop_voffset : in, the crop hsize (12 bit)*
* crop_vsize : in, the crop vsize (12 bit)*
* *
* Returns : *
* Error message *
* *
* See also : *
* *
***************************************************************************************/
UINT8
HAL_CdspCropGet(
	UINT8* crop_henable,
	UINT8* crop_venable,
	UINT16* crop_hoffset,
	UINT16* crop_hsize,
	UINT16* crop_voffset,
	UINT16* crop_vsize
)
{
	// UINT16 i;

	*crop_henable = XBYTE[ REG_Cdsp_Cropen ] & 0x01;
	*crop_venable = ( XBYTE[ REG_Cdsp_Cropen ] & 0x02 ) >> 1;

	*crop_hoffset = ( UINT16 ) ( XBYTE[ REG_Cdsp_pcrophoffset_high ] >> 8 ) | XBYTE[ REG_Cdsp_pcrophoffset_low ];
	*crop_voffset = ( UINT16 ) ( XBYTE[ REG_Cdsp_pcropvoffset_high ] >> 8 ) | XBYTE[ REG_Cdsp_pcropvoffset_low ];
	*crop_hsize = ( UINT16 ) ( XBYTE[ REG_Cdsp_pcrophsize_high ] >> 8 ) | XBYTE[ REG_Cdsp_pcrophsize_low ];
	*crop_vsize = ( UINT16 ) ( XBYTE[ REG_Cdsp_pcropvsize_high ] >> 8 ) | XBYTE[ REG_Cdsp_pcropvsize_low ];

	return HALK_SUCCESS;
}

/**************************************************************************************
* *
* Function Name : HAL_DisableLutgammavalidsignal*
* *
* Purposes : *
* To Disable lutgamma valid signal if raw save to Dram *
* *
* Desprictions : *
* Disable lutgamma valid signal if raw save to Dram*
* *
* Arguments : *
* intlvalidinon : in, Disable lutgamma valid signal if raw save to Dram (1 bit)
*
* *
* Returns : *
* None *
* *
* See also : *
* *
***************************************************************************************/
UINT8
HAL_CdspSpecialeffectSet(
	UINT8 intlvalidinon
)
{
	XBYTE[ REG_Cdsp_intlvalidinon ] = ( intlvalidinon & 0x01 );

	return HALK_SUCCESS;
}

/**************************************************************************************
* *
* Function Name : HAL_CdspSpeffectSet *
* *
* Purposes : *
* To configure specail effect parameter *
* *
* Desprictions : *
* Set paramaters of specail effect *
* *
* Arguments : *
* specmode : in, Special effect mode (RGB Bayer input) (3 bit)
*   			 0: Normal
*   			 1: Negtive
*   			 2: Solarise
*   			 3: Binarize
*
* specsvden : in, Written specmode register value update (1 bit)
*   			  0: immediate update
*   			  1: reflected at vdupdate & VD falling edge
*
* *
* Returns : *
* None *
* *
* See also : *
* *
***************************************************************************************/
UINT8
HAL_CdspSpeffectSet(
	UINT8 specmode,
	UINT8 specsvden
)
{
	XBYTE[ REG_Cdsp_specmode ] = ( ( specsvden & 0x10 ) << 4 ) | ( specmode & 0x07 );

	return HALK_SUCCESS;
}

/**************************************************************************************
* *
* Function Name : HAL_CDSPBpCompensation *
* *
* Purposes : *
* Set Bad pixel compensation function. *
* *
* Desprictions : *
* Set Bad pixel compensation function. *
* *
* Arguments : *
* badpixen : in, Bad pixel compensation enable
* badpixtesten : in, Bad pixel test mode*
* bphitcount : out, Bad pixel hit counter
* badbaseaddr : in, The bad pixel address in SDRAM (Word Unit)
* Returns : *
* None *
* *
* See also : *
* *
***************************************************************************************/
UINT8
HAL_CdspBpCompensation(
		UINT8 badpixen,
		UINT8 badpixtesten,
		UINT16* bphitcount,
		UINT32 badbaseaddr
)
{
	//badpixtesten=0;
	//* bphitcount=0;
	//badpixen=0;
	
	XBYTE[ REG_Cdsp_badbaseaddr_byte0 ] = badbaseaddr & 0xff;
	XBYTE[ REG_Cdsp_badbaseaddr_byte1 ] = ( ( badbaseaddr >> 8 ) & 0xFF );
	XBYTE[ REG_Cdsp_badbaseaddr_byte2 ] = ( ( badbaseaddr >> 16 ) & 0xFF );
	XBYTE[ REG_Cdsp_badbaseaddr_byte3 ] = ( ( badbaseaddr >> 24 ) & 0xFF );
	#if 0
		printf("XBYTE[REG_Cdsp_badbaseaddr_byte0]= =0x%bx \n",XBYTE[REG_Cdsp_badbaseaddr_byte0]);
		printf("XBYTE[REG_Cdsp_badbaseaddr_byte1]= =0x%bx \n",XBYTE[REG_Cdsp_badbaseaddr_byte1]);
		printf("XBYTE[REG_Cdsp_badbaseaddr_byte2]= =0x%bx \n",XBYTE[REG_Cdsp_badbaseaddr_byte2]);
		printf("XBYTE[REG_Cdsp_badbaseaddr_byte3]= =0x%bx \n",XBYTE[REG_Cdsp_badbaseaddr_byte3]);
	#endif
	
	XBYTE[ REG_Cdsp_badpixen ] = ( badpixtesten & 0x03 ) << 4 | ( badpixen & 0x01 );
	//XBYTE[REG_Cdsp_bphitcount_low]=LowByte(bphitcount);
	//XBYTE[REG_Cdsp_bphitcount_high]=HighByte(bphitcount);
	( UINT16 ) ( *bphitcount ) = ( UINT16 ) ( XBYTE[ REG_Cdsp_bphitcount_high ] << 8 ) | ( XBYTE[ REG_Cdsp_bphitcount_low ] );
	//	printf("bphitcount =0x%x \n",*bphitcount);
	
	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspHscaleRGBEnable(UINT8 hscale_enable, UINT8 hscale_mode, UINT8 hscale_sync_mode)
 * @brief     Enable/Disable the Hscale function in RGB domain and set the sync method
 * @param     [in] hscale_enable : the horizontal scale enable/disable.(1 bit)
 * @param     [in] hscale_mode : Horizontal raw data scale down mode (1 bit)
 *									0: drop
 *									1: filter
 * @param     [in] hscale_sync_mode : hscale sync method (2 bit)
 *									0: immediate update
 *									1: reflected at the next valid VD edge
 *									2: reflected at the zoomfactor update
 *					(hscalesvden : in, Rewritten factor register value update method (1 bit)
 *									0: immediate update
 *									1: reflected at the next valid VD edge)
 *					(hscaleszfen : in,  0:depend on hscalesvden ,			   (1 bit)
 *									1: reflected at the zoomfactor update)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/

UINT8
HAL_CdspHscaleRGBEnable(
	UINT8 hscale_enable,
	UINT8 hscale_mode,
	UINT8 hscale_sync_mode
)
{
	XBYTE[ REG_Cdsp_HRGBscaleen ] = ( ( hscale_mode & 0x01 ) << 4 ) | ( hscale_enable & 0x01 );
	/*XBYTE[REG_Cdsp_HRGBscalesyncen] = ((hscaleszfen & 0x01)<<4 )|( hscalesvden & 0x01);*/
	if( hscale_sync_mode >= 2 ) {
		XBYTE[ REG_Cdsp_HRGBscalesyncen ] |= 0x10;
	}
	else {
		XBYTE[ REG_Cdsp_HRGBscalesyncen ] |= ( hscale_sync_mode & 0x01 );
	}

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspVDramMode(UINT8 Mode, UINT8 ModeSVd)
 * @brief     Set CDSP Image format when write to SDRAM
 * @param     [in] mode
 *						0 -> YUV 422 Format
 *						1 -> Raw 8 Bits Format
 *						2 -> Raw 10 Bits Mode
 * @param     [in] modeSVd
 *						0 -> Immidate Update
 *						1 -> reflected at VD falling edge
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspVDramMode(
	UINT8 mode,
	UINT8 modeSVd
)
{
	XBYTE[ REG_Cdsp_imgmode ] = ( mode & 0x03 ) | ( ( modeSVd & 0x01 ) << 4 );

	return HALK_SUCCESS;
}

#endif


#if 0
UINT8 HAL_AWBWindowRead1024(void)
{

  	UINT16 i,j,index;

	UINT8  lbyte, hbyte;
	AWB_valuet AWB_resultvalue;
//printf(" AWB_resultvalue = 0x%x \n",(UINT32)AWBWin );
//	pAWB_resultvalue = (UINT32)AWBWin;

	//  printf(" AWB_resultvalue = 0x%x \n",(UINT32)AWBWin );
//  	printf(" pAWB_resultvalue = 0x%x \n",(UINT32)pAWB_resultvalue );
	for ( i=0; i<32; i++)
	  {
			for ( j=0; j<32 ; j++)
			{
	//  		 printf("i=%d,j=%d\n");
			   index = (i*64)+j;
//			   HAL_DRAMindirectread(AWBAddr+index, &yval);
			   M_DRAM_SetStartAddress((UINT32)(AWBAddr+index), 1);
					 M_DRAM_ReadWord(lbyte, hbyte);
			  // lbyte = (UINT8)(yval & 0xFF);
			  // hbyte = (UINT8)((yval >> 8) & 0xFF);
			   AWB_resultvalue.R_Value= hbyte   ;

			   AWB_resultvalue.G_Value= lbyte;

			   M_DRAM_SetStartAddress(AWBAddr+index+32, 1);
					 M_DRAM_ReadWord(lbyte, hbyte);


			   AWB_resultvalue.G_Value += hbyte;
			   AWB_resultvalue.G_Value=AWB_resultvalue.G_Value>>1;
			   AWB_resultvalue.B_Value= lbyte;


   if ( i*32+j >1000 )
   {
	printf("AWB_resultvalue[%d].R=0x%x \n",i*32+j,AWB_resultvalue.R_Value);
	printf("AWB_resultvalue[%d].G=0x%x \n",i*32+j,AWB_resultvalue.G_Value);
	printf("AWB_resultvalue[%d].B=0x%x \n",i*32+j,AWB_resultvalue.B_Value);

	}


			}
	  }
	  return 0;
}
#endif

#if 0
UINT8
halCdspDoYuvHScaling(
	UINT16 srcXSize,
	UINT16 srcYSize,
	UINT16 dstXSize,
	UINT16 dstYSize
)
{
	UINT32 f, hRmder;
	UINT16 hF, hFI;

	XBYTE[ 0x226E ] = 0x88;
	XBYTE[ 0x226f ] = 0x00;

	// disable V scaling
	XBYTE[ REG_Cdsp_YUVscalesvden ] = 0x00;
	XBYTE[ 0x21c8 ] = 0x00;
	XBYTE[ 0x21d0 ] = 0x00;

	f = ( ( ( UINT32 ) dstXSize << 16 ) / ( UINT32 ) srcXSize );
	hRmder = ( ( UINT32 ) 65536 ) % f;

	if ( hRmder != 0 ) {
		f++;
	}

	hF = (UINT16)f;
	hFI = hF;

	// H scaling
	XBYTE[ REG_Cdsp_YUVhscaleen ] = 0x11;
	XBYTE[ REG_Cdsp_YUVhscalefactor_low ] = LowByte( hF );
	XBYTE[ REG_Cdsp_YUVhscalefactor_high ] = HighByte( hF );
	XBYTE[ REG_Cdsp_Yhscaleaccini_low ] = LowByte( hF );
	XBYTE[ REG_Cdsp_Yhscaleaccini_high ] = HighByte( hF );

	// DramV
	XBYTE[ REG_Cdsp_mirrorline ] = 0x00;
	XBYTE[ REG_Cdsp_Skippixel ] = 0x00;

	// size
	XBYTE[ REG_Cdsp_Rhoffset_low ] = 0x00;
	XBYTE[ REG_Cdsp_Rhoffset_high ] = 0x00;
	XBYTE[ REG_Cdsp_Rhsize_low ] = READ8( srcXSize, 1 );
	XBYTE[ REG_Cdsp_Rhsize_high ] = READ8( srcXSize, 0 );

	XBYTE[ REG_Cdsp_Rvoffset_low ]  = 0x00;
	XBYTE[ REG_Cdsp_Rvoffset_high ] = 0x00;
	XBYTE[ REG_Cdsp_Rvsize_low ]  = READ8( srcYSize, 1 );
	XBYTE[ REG_Cdsp_Rvsize_high ] = READ8( srcYSize, 0 );

	// VDram
	XBYTE[ REG_Cdsp_imgmode ] = 0x00;

	XBYTE[ REG_Cdsp_Whoffset_low ] = 0x00;
	XBYTE[ REG_Cdsp_Whoffset_high ] = 0x00;

	XBYTE[ REG_Cdsp_Wvoffset_low ]  = 0x00;
	XBYTE[ REG_Cdsp_Wvoffset_high ] = 0x00;

	HAL_CdspVDramSizeClamp( 1, dstXSize, 1, dstYSize, 0 );  /* Clamp the size to SDRAM */

	XBYTE[ REG_Cdsp_Remode ] = 0x12;   /* Set DoCDSP = 1 */

	while( ( XBYTE[ REG_Cdsp_Remode ] & 0x10 ) == 0x10 );

	return 0;
}
#endif

#if 0
UINT8
HAL_AWBWindowRead256(
	void
)
{
#if 0
  	UINT16 i,j,index;

	UINT8  lbyte, hbyte;
	AWB_valuet AWB_resultvalue;
//printf(" AWB_resultvalue = 0x%x \n",(UINT32)AWBWin );
//	pAWB_resultvalue = (UINT32)AWBWin;

	//  printf(" AWB_resultvalue = 0x%x \n",(UINT32)AWBWin );
//  	printf(" pAWB_resultvalue = 0x%x \n",(UINT32)pAWB_resultvalue );
	for ( i=0; i<16; i++)
	  {
			for ( j=0; j<16 ; j++)
			{
	//  		 printf("i=%d,j=%d\n");
			   index = (i*32)+j;
//			   HAL_DRAMindirectread(AWBAddr+index, &yval);
			   M_DRAM_SetStartAddress((UINT32)(AWBAddr+index), 1);
					 M_DRAM_ReadWord(lbyte, hbyte);
			  // lbyte = (UINT8)(yval & 0xFF);
			  // hbyte = (UINT8)((yval >> 8) & 0xFF);
			   	AWB_resultvalue.R_Value= hbyte   ;
			   AWB_resultvalue.G_Value= lbyte;
			   M_DRAM_SetStartAddress(AWBAddr+index+16, 1);
					 M_DRAM_ReadWord(lbyte, hbyte);

			   AWB_resultvalue.G_Value += hbyte;
			   AWB_resultvalue.G_Value=AWB_resultvalue.G_Value>>1;
			   AWB_resultvalue.B_Value= lbyte;

   if ( i*16+j >0 )
   {
	printf("AWB_resultvalue[%d].R=0x%x \n",i*16+j,AWB_resultvalue.R_Value);
	printf("AWB_resultvalue[%d].G=0x%x \n",i*16+j,AWB_resultvalue.G_Value);
	printf("AWB_resultvalue[%d].B=0x%x \n",i*16+j,AWB_resultvalue.B_Value);
	}
		}
	}
#else

	//UINT8	y[1024];
	//UINT16 j;
	#if 0
	XBYTE[REG_Dram_IndirectAddr_B0] = XBYTE[REG_Cdsp_AWBWinaddr_Byte0];//set SDRAM starting addr
	XBYTE[REG_Dram_IndirectAddr_B1] = XBYTE[REG_Cdsp_AWBWinaddr_Byte1];
	XBYTE[REG_Dram_IndirectAddr_B2] = XBYTE[REG_Cdsp_AWBWinaddr_Byte2];
	XBYTE[REG_Dram_IndirectAddr_B3] = XBYTE[REG_Cdsp_AWBWinaddr_Byte3];
	printf("AWBWinaddr = 0x%bx%bx%bx%bx\n",XBYTE[REG_Cdsp_AWBWinaddr_Byte3],XBYTE[REG_Cdsp_AWBWinaddr_Byte2],XBYTE[REG_Cdsp_AWBWinaddr_Byte1],XBYTE[REG_Cdsp_AWBWinaddr_Byte0]);
	XBYTE[REG_Dram_IndirectPrefetch] = 0x01;

	for (j = 0; j<=511; j+=1) {
		while(XBYTE[REG_Dram_IndirectPrefetch]);
		y[j*2]	=XBYTE[REG_Dram_IndirectRData_Low];
		y[j*2+1]=XBYTE[REG_Dram_IndirectRData_High];
			}
	#else
	HAL_GprmDramDma((K_SDRAM_PvAwbWndAddr<<1),(K_SDRAM_CodeSize + 16 * 1024) << 1,1024,1);
	#endif
	#if 0

	printf("\n{\n");
	for (j = 0; j<=1023; j+=1) {
		if((j%32)==0)
			printf("\n");
		printf ("%bx,", AWBBuff[j]);
	  }
	printf("\n}\n");
	#endif

#endif

	return 0;
}
#endif

#if 0
//Dummy for compiler
UINT8
HAL_SetCDSPScale(
	UINT16 a,
	UINT16 b
)
{
	a = 0;
	b = 0;

	return 0;
}
#endif

#if 0
UINT8
HAL_ConfigWindow(
	UINT8 a,
	UINT8 b,
	UINT8 c,
	UINT16 d
)
{
	a = 0;
	b = 0;
	c = 0;
	d = 0;

	return 0;
}
#endif

#if 0
void
HAL_StillWB(
	UINT32 rawBufAddr,
	UINT16 rawXSize,
	UINT16 rawYSize
)
{
	UINT32 i, j;
	UINT8 x = 0, y = 0, z = 0;
	UINT8 R[ 256 ], G[ 256 ], B[ 256 ];
	UINT8 lbyte, hbyte;
	//unsigned gridsize;
	UINT8 filter_state;


	//printf("rawBufAddr = 0x%lx ( %u x %u )\n",rawBufAddr,rawXSize,rawYSize);
	//XBYTE[0x2030]|=0x01;
	for( j = 2; j < rawYSize; j += 128 ) {
		for( i = 2; i < rawXSize; i += 86 ) {
			z = x * 16 + y;
			M_DRAM_SetStartAddress( ( UINT32 ) ( rawBufAddr + i + j * 1296 ), 1 );
			M_DRAM_ReadWord( lbyte, hbyte );
			R[ z ] = ( hbyte );
			G[ z ] += ( lbyte >> 1 );
			//printf ("1.Raw%bu (%lu,%lu) (%lu)= (%bx, %bx) \n",z, i,j,i+j*1296, hbyte, lbyte);
			M_DRAM_SetStartAddress( ( UINT32 ) ( rawBufAddr + i + ( j + 1 ) * 1296 ), 1 );
			M_DRAM_ReadWord( lbyte, hbyte );
			G[ z ] += ( hbyte >> 1 );
			B[ z ] = ( lbyte );
			//printf ("2.Raw%bu (%lu,%lu) (%lu)= (%bx, %bx) \n",z, i,j,i+(j+1)*1296, hbyte, lbyte);
			//printf("y = %bu\n",y);
			if( y == 15 )
				y = 0;
			else
				y++;
		}
		//printf("x = %bu\n",x);
		if( x == 15 )
			x = 0;
		else
			x++;
	}
	//XBYTE[0x2030]&=(~0x01);

#if 1
	//filter_state = 0;
	for( j = 0; j < 256; j++ ) {
		//XBYTE[ 0x2030 ] |= 0x01;
		filter_state = awbWhitePointFilter( R[ j ], G[ j ], B[ j ] );
		//XBYTE[ 0x2030 ] &= ~0x01;
	}
	//gridsize = 256;
	awbRBGainDecider( 0, 256 );
	awbCalConvergeGain();
#endif
}
#endif

#if 0
void AWBInit()
{
	//    UINT16 i;
	//    UINT16 yval;
	//    UINT8  lbyte, hbyte;
	//	UINT8 AWBWin[1024];

	HAL_CdspAWBwinAddrset( K_SDRAM_PvAwbWndAddr );
	HAL_CdspAWBWindowSizeSet( 40, 680, 30, 450, 32 ,32);
	HAL_CdspVDramSkipWr( 0, 0 );
	HAL_AWBWindowRead256();
}
void AEWinInit( void )
{
//	UINT8 AEWin[ 64 ];
	//XBYTE[0x2ae0]=0x80;//white
	//XBYTE[0x2ae0]=0x87;//black
	//XBYTE[0x2ae0]=0x83;//green
	//XBYTE[0x2ae0]=0x85;//red
	//XBYTE[0x2ae0]=0x86;//blue
	HAL_CdspAEWinEnSet( 1, 1, 0 );
	HAL_CdspVDramSkipWr( 0, 0 );
	HAL_CdspAEwinintvlset( 0, 0 );
	HAL_CdspAEwinAddrset( K_SDRAM_PvAeWndAddr );
	XBYTE[REG_Cdsp_AEwincnt]=0x77;
	HAL_CdspAEWinEnSet( 1, 1, 1 );
	//serialCmdQueInit();
}
#endif


