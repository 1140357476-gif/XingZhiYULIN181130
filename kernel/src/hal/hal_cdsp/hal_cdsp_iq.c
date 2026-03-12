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
							  
#include "sp1k_cdsp_api.h"
#include "sp1k_calibration_api.h"
#include "util_api.h"
#include "front_logi.h"
							  
#include "hal_cdsp.h"   
#include "hal_cdsp_core.h" 
#include "hal_cdsp_internal.h" 

#include "hal_front.h"
#include "reg_def.h"
#include "hal_dram.h"
#include "snap_core.h"

	 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define _DBG_WITH_ALL_FUNC_

#define __FILE	__FILE_ID_HAL_CDSP__
#define HAL_CDSP_IQ_TAG_TODO
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
static xdata UINT8 halCdspReg21b0;
static xdata UINT8 halCdspReg21b4;
static xdata UINT8 halCdspReg21b5;
static xdata UINT8 halCdspReg21b6;
static xdata UINT8 halCdspReg21b7;
static xdata UINT8 halCdspReg21b8;
static xdata UINT8 halCdspReg21b9;
static xdata UINT8 halCdspReg21ba;
static xdata UINT8 halCdspReg21bc;
static xdata UINT8 halCdspReg21bd;
			
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
		
extern xdata UINT8 calibrationAWBOn; 


/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/


/**
 * @fn        void HAL_CdspBayerPatternSet(UINT8 ptn)
 * @brief     Set Bayer Pattern 
 * @param     [in] ptn : bayer pattern (2 bits)
 *						0 :
 *						1 :
 *						2 :
 *						3 : 
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_CdspBayerPatternSet(
	UINT8 ptn
)
{
	//printf("pattern=0x%02bx\n", ptn);
	halCdspBayerPtn = ptn;

	XBYTE[REG_Cdsp_pixsel] = ptn;
	XBYTE[REG_Cdsp_CorePixSel] = ptn;
}


/**
 * @fn        UINT8 HAL_CdspColorMtxSet(UINT16 a00, UINT16 a01, UINT16 a02, UINT16 a10, UINT16 a11, UINT16 a12, UINT16 a20, UINT16 a21, UINT16 a22)
 * @brief     To set color correction matrix parameters
 * @param     [in] a00 : Coefficient 00 for color correction
 * @param     [in] a01 : Coefficient 01 for color correction
 * @param     [in] a02 : Coefficient 02 for color correction
 * @param     [in] a10 : Coefficient 10 for color correction
 * @param     [in] a11 : Coefficient 11 for color correction
 * @param     [in] a12 : Coefficient 12 for color correction
 * @param     [in] a20 : Coefficient 20 for color correction
 * @param     [in] a21 : Coefficient 21 for color correction
 * @param     [in] a22 : Coefficient 22 for color correction
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspColorMtxSet(
	UINT16 a00,
	UINT16 a01,
	UINT16 a02,
	UINT16 a10,
	UINT16 a11,
	UINT16 a12,
	UINT16 a20,
	UINT16 a21,
	UINT16 a22
)
{
	XBYTE[ REG_Cdsp_ColorM_A00_low ] = LowByte( a00 );
	XBYTE[ REG_Cdsp_ColorM_A00_high ] = HighByte( a00 );

	XBYTE[ REG_Cdsp_ColorM_A01_low ] = LowByte( a01 );
	XBYTE[ REG_Cdsp_ColorM_A01_high ] = HighByte( a01 );

	XBYTE[ REG_Cdsp_ColorM_A02_low ] = LowByte( a02 );
	XBYTE[ REG_Cdsp_ColorM_A02_high ] = HighByte( a02 );

	XBYTE[ REG_Cdsp_ColorM_A10_low ] = LowByte( a10 );
	XBYTE[ REG_Cdsp_ColorM_A10_high ] = HighByte( a10 );

	XBYTE[ REG_Cdsp_ColorM_A11_low ] = LowByte( a11 );
	XBYTE[ REG_Cdsp_ColorM_A11_high ] = HighByte( a11 );

	XBYTE[ REG_Cdsp_ColorM_A12_low ] = LowByte( a12 );
	XBYTE[ REG_Cdsp_ColorM_A12_high ] = HighByte( a12 );

	XBYTE[ REG_Cdsp_ColorM_A20_low ] = LowByte( a20 );
	XBYTE[ REG_Cdsp_ColorM_A20_high ] = HighByte( a20 );

	XBYTE[ REG_Cdsp_ColorM_A21_low ] = LowByte( a21 );
	XBYTE[ REG_Cdsp_ColorM_A21_high ] = HighByte( a21 );

	XBYTE[ REG_Cdsp_ColorM_A22_low ] = LowByte( a22 );
	XBYTE[ REG_Cdsp_ColorM_A22_high ] = HighByte( a22 );

	return HALK_SUCCESS;
}


/**
 * @fn        UINT8 HAL_CdspBriContEnSet(UINT8 ybycen)
 * @brief     enable/disable brightness/contrast engine
 * @param     [in] ybycen : Y brightness & Y contrast adjustment enable (1 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspBriContEnSet(
	UINT8 ybycen
)
{

	XBYTE[ REG_Cdsp_YbYcen ] = (XBYTE[ REG_Cdsp_YbYcen ]& 0xfe) | (ybycen&0x01) ;

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspYThrEnSet(UINT8 ythren)
 * @brief     enable/disable Y threshold
 * @param     [in] ythren : Y threshold enable (1 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspYThrEnSet(
	UINT8 ythren
)
{
	XBYTE[ REG_Cdsp_YbYcen ] = (XBYTE[ REG_Cdsp_YbYcen ]& 0xfd) | ((ythren&0x01)<<1) ;
	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspYRevrseEnSet(UINT8 yreven)
 * @brief     enable/disable Y reverse 
 * @param     [in] yreven : Y reverse enable (1 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspYRevrseEnSet(
	UINT8 yreven
)
{
	XBYTE[ REG_Cdsp_YbYcen ] = (XBYTE[ REG_Cdsp_YbYcen ]& 0xfb) | ((yreven&0x01)<<2) ;
	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspBriContEnSet(UINT8 ybycen, UINT8 ythren, UINT8 yreven, UINT8 uvsathuen, UINT8 yb, UINT8 yc, UINT8 ythr)
 * @brief     set Y threshold value
 * @param     [in] ythr : Y threshold(8 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspYThrSet(
	UINT8 ythr
)
{

	XBYTE[ REG_Cdsp_Ythr ] = ythr;

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspYBrightOffsetSet(UINT8 yb)
 * @brief     HAL_CdspYBrightOffsetSet
 * @param     [in] yb : Y brightness offset (2's complement, -128~127)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspYBrightOffsetSet(
	SINT8 yb
)
{
	XBYTE[ REG_Cdsp_Yb ] = yb;
	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspYContrastOffsetSet(UINT8 yc)
 * @brief     HAL_CdspYContrastOffsetSet
 * @param     [in] yc : Y contrast factor (3.5 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspYContrastOffsetSet(
	UINT8 yc
)
{
	XBYTE[ REG_Cdsp_Yc ] = yc;
	return HALK_SUCCESS;
}




/**
 * @fn        UINT8 HAL_CdspSatHueEnSet(UINT8 enable)
 * @brief     HAL_CdspSatHueEnSet
 * @param     [in] enable : UV Saturation and hue adjustment enable (1 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspSatHueEnSet(
	UINT8 enable
)
{
	XBYTE[ REG_Cdsp_YbYcen ] = ( XBYTE[ REG_Cdsp_YbYcen ] & 0xef ) | ( ( enable & 0x01 ) << 4 );

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspSatHueParaSet(UINT8 enable, UINT8 uvsat, UINT8 huesin1data, UINT8 huecosdata, UINT8 huesin2data, UINT8 UOffset, UINT8 VOffset)
 * @brief     set UV saturation & hue
 * @param     [in] uvsat : UV saturation factor (3.5 bit)
 * @param     [in] huesin1data : Hue sin data (2.6 one sign bit)
 * @param     [in] huecosdata : Hue cos data (2.6 one sign bit)
 * @param     [in] huesin2data : Hue sin data (2.6 one sign bit)
 * @param     [in] UOffset : U Offset ( -128 ~ 127 )
 * @param     [in] VOffset : V Offset ( -128 ~ 127 )
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspSatHueParaSet(
	UINT8 uvsat,
	UINT8 huesin1data,
	UINT8 huecosdata,
	UINT8 huesin2data,
	UINT8 uOffset,
	UINT8 vOffset
)
{	
	XBYTE[ REG_Cdsp_UVsat ] = uvsat;
	XBYTE[ REG_Cdsp_huesin1data ] = huesin1data;
	XBYTE[ REG_Cdsp_huecosdata ] = huecosdata;
	XBYTE[ REG_Cdsp_huesin2data ] = huesin2data;
	XBYTE[ REG_Cdsp_UOffset ] = uOffset;
	XBYTE[ REG_Cdsp_VOffset ] = vOffset;

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspWBGainSet(UINT16 rGain, UINT16 grGain, UINT16 bGain, UINT16 gbGain)
 * @brief     Set the gain of R, Gr, B, Gb raw data
 * @param     [in] rGain : R gain for white balance (5.6 bit) 
 * @param     [in] grGain : Gr gain for white balance (5.6 bit)
 * @param     [in] bGain : B gain for white balance (5.6 bit)
 * @param     [in] gbGain : Gb gain for white balance (5.6 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/

UINT8
HAL_CdspWBGainEnSet(
	UINT8 enable
)
{
	

	XBYTE[ REG_Cdsp_gain_en ] = (enable&0x01);

	
	return HALK_SUCCESS;
}


/**
 * @fn        UINT8 HAL_CdspWBGainEnGet(void)
 * @brief     Get WB Gain status
 * @param     NONE
 * @retval    return = WB Gain Enable / Disable
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspWBGainEnGet(
	void
)
{
	return	XBYTE[ REG_Cdsp_gain_en ]&0x01;
}


/**
 * @fn        UINT8 HAL_CdspWBOffsetEnSet(UINT8 enable)
 * @brief     HAL_CdspWBOffsetEnSet
 * @param     [in] enable : enable WB Offset
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspWBOffsetEnSet(
	UINT8 enable
)
{
	XBYTE[ REG_Cdsp_wboffseten ] = ( XBYTE[ REG_Cdsp_wboffseten ] & ~0x01 ) | enable;

	return 0;
}

/**
 * @fn        UINT8 HAL_CdspWBOffsetEnGet(void)
 * @brief     HAL_CdspWBOffsetEnGet
 * @param     NONE
 * @retval    return = WB Offset Enable / Disable
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspWBOffsetEnGet(
	void
)
{
	UINT8 enable = ( XBYTE[ REG_Cdsp_wboffseten ] & 0x01 );

	return enable;
}

/**
 * @fn        UINT8 HAL_CdspWBGainSet(UINT16 rGain, UINT16 grGain, UINT16 bGain, UINT16 gbGain)
 * @brief     HAL_CdspWBGainSet
 * @param     [in] rGain (5.6 bits)
 * @param     [in] grGain (5.6 bits)
 * @param     [in] bGain (5.6 bits)
 * @param     [in] gbGain (5.6 bits)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspWBGainSet(
	UINT16 rGain,
	UINT16 grGain,
	UINT16 bGain,
	UINT16 gbGain
) 
{
	XBYTE[ REG_Cdsp_prgain_low ] = LowByte( rGain );
	XBYTE[ REG_Cdsp_prgain_high ] = HighByte( rGain );

	XBYTE[ REG_Cdsp_pgrgain_low ] = LowByte( grGain );
	XBYTE[ REG_Cdsp_pgrgain_high ] = HighByte( grGain );

	XBYTE[ REG_Cdsp_pbgain_low ] = LowByte( bGain );
	XBYTE[ REG_Cdsp_pbgain_high ] = HighByte( bGain );

	XBYTE[ REG_Cdsp_pgbgain_low ] = LowByte( gbGain );
	XBYTE[ REG_Cdsp_pgbgain_high ] = HighByte( gbGain );

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspWBGainGet(UINT8 *rGain, UINT8 *grGain, UINT8 *bGain, UINT8 *gbGain)
 * @brief     Get CDSP white balance gain
 * @param     [in] rGain : R gain value (5.6 bits)
 * @param     [in] grGain : Gr gain value (5.6 bits)
 * @param     [in] bGain : B gain value (5.6 bits)
 * @param     [in] gbGain : Gb gain value (5.6 bits)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspWBGainGet(
	UINT8 *rGain,
	UINT8 *grGain,
	UINT8 *bGain,
	UINT8 *gbGain
) USING_0
{
	*rGain  = XBYTE[ REG_Cdsp_prgain_low ];
	*grGain = XBYTE[ REG_Cdsp_pgrgain_low ];
	*bGain  = XBYTE[ REG_Cdsp_pbgain_low ];
	*gbGain = XBYTE[ REG_Cdsp_pgbgain_low ];

	return HALK_SUCCESS;
}



/**
 * @fn        UINT8 HAL_CdspWBOffsetCfgSet(UINT8 wbOffsetEn, UINT8 gOsVden, UINT8 wbOValidOn)
 * @brief     HAL_CdspWBOffsetCfgSet
 * @param	  [in] wbOffsetEn : Optical black & channel offset function (protect re-subtraction for CDSP re-processing) (1 bit)
 *								0: disable
 *								1: enable
 * @param	  [in] gOsVden : Rewritten gain & offset register value update method  (1 bit)
 *								0: immediate update
 *								1: reflected at vdupdate & VD falling edge
 * @param     [in] wbOValidOn : Disable wboffset valid signal if YUV sensor input (1 bit) 
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-7-16
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspWBOffsetCfgSet(
	UINT8 gOsVden,
	UINT8 wbOValidOn
)
{
	
	XBYTE[ REG_Cdsp_wboffseten ] = ( XBYTE[ REG_Cdsp_wboffseten ] & ~0x10 ) | ( ( gOsVden & 0x01 ) << 4 );
	
	XBYTE[ REG_Cdsp_wbovalidon ] = wbOValidOn & 0x01;

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspWBOffsetSet(UINT8 wbOffsetEn, UINT8 gOsVden, UINT16 rOffset, UINT16 grOffset, UINT16 bOffset, UINT16 gbOffset, UINT8 wbOValidOn)
 * @brief     Set the offset of R, Gr, B, Gb raw data
 * @param     [in] rOffset : The offset of R raw data  (1 sign +9 bit)
 * @param     [in] grOffset : The offset of Gr raw data  (1 sign +9 bit)
 * @param     [in] bOffset : The offset of B raw data  (1 sign +9 bit)
 * @param     [in] gbOffset : The offset of Gb raw data  (1 sign +9 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspWBOffsetSet(
	UINT16 rOffset,
	UINT16 grOffset,
	UINT16 bOffset,
	UINT16 gbOffset
) 
{
	XBYTE[ REG_Cdsp_proffset_low ] = LowByte( rOffset );
	XBYTE[ REG_Cdsp_proffset_high ] = HighByte( rOffset );
	XBYTE[ REG_Cdsp_pgroffset_low ] = LowByte( grOffset );
	XBYTE[ REG_Cdsp_pgroffset_high ] = HighByte( grOffset );
	XBYTE[ REG_Cdsp_pboffset_low ] = LowByte( bOffset );
	XBYTE[ REG_Cdsp_pboffset_high ] = HighByte( bOffset );
	XBYTE[ REG_Cdsp_pgboffset_low ] = LowByte( gbOffset );
	XBYTE[ REG_Cdsp_pgboffset_high ] = HighByte( gbOffset );


	return HALK_SUCCESS;
}


/**
 * @fn        UINT8 HAL_CdspWBOffsetGet(UINT16 *rOffset, UINT16 *grOffset, UINT16 *bOffset, UINT16 *gbOffset)
 * @brief     Get the offset of R, Gr, B, Gb raw data
 * @param     [in] rOffset
 * @param     [in] grOffset
 * @param     [in] bOffset
 * @param     [in] gbOffset
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-7-16
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspWBOffsetGet(
	UINT16 *rOffset,
	UINT16 *grOffset,
	UINT16 *bOffset,
	UINT16 *gbOffset
)
{
	WRITEP8(rOffset,1,XBYTE[ REG_Cdsp_proffset_low]);
	WRITEP8(rOffset,0,XBYTE[ REG_Cdsp_proffset_high ]);
	WRITEP8(grOffset,1,XBYTE[ REG_Cdsp_pgroffset_low]);
	WRITEP8(grOffset,0,XBYTE[ REG_Cdsp_pgroffset_high ]);
	WRITEP8(bOffset,1,XBYTE[ REG_Cdsp_pboffset_low]);
	WRITEP8(bOffset,0,XBYTE[ REG_Cdsp_pboffset_high ]);
	WRITEP8(gbOffset,1,XBYTE[ REG_Cdsp_pgboffset_low]);
	WRITEP8(gbOffset,0,XBYTE[ REG_Cdsp_pgboffset_high ]);
		
	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspGammaEnSet(UINT8 enable)
 * @brief     Enable or disable gamma module
 * @param     [in] enable : 
 *							0 : disable
 *							1 : enable
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspGammaEnSet(
	UINT8 enable
)
{
    //UINT8 val;	
	//val= XBYTE[ REG_Cdsp_Lutgammaen ];
	//val &= ~0x01;	
	//XBYTE[ REG_Cdsp_Lutgammaen ] = (val | enable);

	XBYTE[ REG_Cdsp_Lutgammaen ] = ((XBYTE[ REG_Cdsp_Lutgammaen ]& ~0x01) | enable);

	return 0;
}

/**
 * @fn        UINT8 HAL_CdspGammaEnGet(void)
 * @brief     Get status of gamma
 * @param     NONE
 * @retval    return = Enable / Disable
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspGammaEnGet(
	void
)
{
    //UINT8 val;
    //val = XBYTE[ REG_Cdsp_Lutgammaen ];
	//val &= 0x01;

	return (XBYTE[ REG_Cdsp_Lutgammaen ]& 0x01);
}




/**
 * @fn        UINT8 HAL_CdspGammalTbInit(UINT8* pGammaData)
 * @brief     Initialize look up table for gamma correction
 * @param     [in] pGammaData : data array for gamma correction look up table ( total 1024 * 8 bit,table 8 bit )
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspGammalTbInit(
	UINT8* pGammaData
)
{
	UINT16 i, j=0;
	UINT8 gammaTransfer[ 4 ];


	XBYTE[ REG_Cdsp_macroSel ] = 0x04;
	XBYTE[ REG_Cdsp_switchclk ] = 0x04;

	//printf("*pgammadata=0x%bx \n",(UINT8)*pgammadata);
	for( i = 0; i < 128; i++ ) {
		
		#if 0 /* for debug */
			printf("pgammadata[0x%x]=0x%bx \n",j,pGammaData[j]);
			printf("pgammadata[0x%x]=0x%bx \n",j+1,pGammaData[j+1]);
			printf("pgammadata[0x%x]=0x%bx \n",j+2,pGammaData[j+2]);
			printf("pgammadata[0x%x]=0x%bx \n",j+3,pGammaData[j+3]);
			printf("pgammadata[0x%x]=0x%bx \n",j+4,pGammaData[j+4]);
			printf("pgammadata[0x%x]=0x%bx \n",j+5,pGammaData[j+5]);
			printf("pgammadata[0x%x]=0x%bx \n",j+6,pGammaData[j+6]);
			printf("pgammadata[0x%x]=0x%bx \n",j+7,pGammaData[j+7]);
		#endif

		#if defined(COMPARE_ANSWER) //|| defined(ICAT_OPTION)
			j = i * 8;
			gammaTransfer[ 0 ] = pGammaData[ j ];
			gammaTransfer[ 1 ] = ( ( ( pGammaData[ j + 4 ] - pGammaData[ j + 3 ] ) & 0x03 ) << 6 ) | ( ( pGammaData[ j + 3 ] - pGammaData[ j + 2 ] ) & 0x03 ) << 4 | ( ( ( ( pGammaData[ j + 2 ] - pGammaData[ j + 1 ] ) ) & 0x03 ) << 2 ) | ( ( ( pGammaData[ j + 1 ] - pGammaData[ j ] ) ) & 0x03 );
			gammaTransfer[ 2 ] = ( ( ( pGammaData[ j + 7 ] - pGammaData[ j + 6 ] ) & 0x03 ) << 4 ) | ( ( ( pGammaData[ j + 6 ] - pGammaData[ j + 5 ] ) & 0x03 ) << 2 ) | ( ( pGammaData[ j + 5 ] - pGammaData[ j + 4 ] ) & 0x03 );
		#else
			j = i * 3;
			gammaTransfer[ 0 ] = pGammaData[ j ];
			gammaTransfer[ 1 ] = pGammaData[ j + 1 ];
			gammaTransfer[ 2 ] = pGammaData[ j + 2 ];			
		#endif
		

		//printf("GammaTra[%03d]=0x%b02x , 0x%b02x , 0x%b02x \n",i,gammaTransfer[0], gammaTransfer[1] ,gammaTransfer[2]);

		XBYTE[ REG_Cdsp_macropagsel ] = 0x00;

		XBYTE[ 0x2b00 + i ] = gammaTransfer[ 0 ];
		while ( XBYTE[ 0x2b00+i ] != gammaTransfer[0]){
		         XBYTE[ 0x2b00+i ] = gammaTransfer[ 0 ];
		}

		//printf("XBYTE[0x%x][b0]=0x%bx \n",0x2b00+i,(UINT8)XBYTE[0x2b00+i]);

		XBYTE[ REG_Cdsp_macropagsel ] = 0x01;

		XBYTE[ 0x2b00 + i ] = gammaTransfer[ 1 ];
	    while ( XBYTE[0x2b00+i] != gammaTransfer[1]){
		        XBYTE[ 0x2b00+i] = gammaTransfer[ 1 ];
	    }
		//printf("XBYTE[0x%x][b1]=0x%bx \n",0x2b00+i,(UINT8)XBYTE[0x2b00+i]);

		XBYTE[ REG_Cdsp_macropagsel ] = 0x02;

		XBYTE[ 0x2b00 + i ] = gammaTransfer[ 2 ];
		while ( XBYTE[0x2b00+i] != gammaTransfer[2]){
		        XBYTE[0x2b00+i] = gammaTransfer[ 2 ];
		}
		
		//printf("XBYTE[0x%x][b2]=0x%bx \n",0x2b00+i,(UINT8)XBYTE[0x2b00+i]);
	}

	XBYTE[ REG_Cdsp_macroSel ] = 0x00;
	XBYTE[ REG_Cdsp_switchclk ] = 0x00;
	XBYTE[ REG_Cdsp_macropagsel ] = 0x00;

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspBPEnSet(UINT8 enable)
 * @brief     Enable/disable bad pixel module
 * @param     [in] enable : 
 *							0: disable
 *							1: enable
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-7
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspBPEnSet(
	UINT8 enable
)
{

	XBYTE[ REG_Cdsp_badpixen ] = ((XBYTE[ REG_Cdsp_badpixen ]& ~0x01) | enable);

	return 0;
}


/**
 * @fn        UINT8 HAL_CdspBPEnGet(void)
 * @brief     Get status of bad pixel
 * @param     NONE
 * @retval    return = 0(disable) / 1(enable)
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-7
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspBPEnGet(
	void
)
{

	return (XBYTE[ REG_Cdsp_badpixen ]& 0x01);
}


/**
 * @fn        UINT8 HAL_CdspLscWinStepSet(UINT8 LSChstep, UINT8 LSCvstep, LSCvstep,UINT8 LSChoincstep, cstep,UINT8 LSCheincstep, cstep,UINT8 LSCvoincstep, cstep,UINT8 LSCveincstep)
 * @brief     Set the Lens compensation step parameters
 * @param     [in] lscHStep : horizontal step (3 bit)
 *\n						  0: 8 , 1:16 , 2:32
 * @param     [in] lscVStep : veritical step (3 bit)
 *\n						  0: 8 , 1:16 , 2:32 
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
HAL_CdspLscWinStepSet(
	UINT8 lscHStep,
	UINT8 lscVStep
)
{
	XBYTE[REG_Cdsp_LscLsstepfact]= ((lscVStep&0x07)<<4 ) |  (lscHStep&0x07);


	return HALK_SUCCESS;
}

/**
 * @fn        void HAL_CdspLscWinStepGet(UINT8 *lscHStep, UINT8 *lscVStep)
 * @brief     HAL_CdspLscWinStepGet
 * @param     [out] lscHStep : horizontal step (3 bit)
 *\n						  0: 8 , 1:16 , 2:32 
 * @param     [out] lscVStep : veritical step (3 bit)
 *\n						  0: 8 , 1:16 , 2:32 
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2011-3-2
 * @todo      N/A
 * @bug       N/A
*/
/*
void 
HAL_CdspLscWinStepGet(
	UINT8 *lscHStep,
	UINT8 *lscVStep
)
{
	UINT8 val;

	val = XBYTE[REG_Cdsp_LscLsstepfact];

	*lscHStep = (val & 0x07);
	
	*lscVStep = ((val & 0x70) >>4);
}
*/

/**
 * @fn        UINT8 HAL_CdspLscWinIncStepSet(UINT8 lscHOIncStep, UINT8 lscHEIncstep, UINT8 lscVOIncStep, UINT8 lscVEIncStep)
 * @brief     HAL_CdspLscWinIncStepSet
 * @param     [in] lscHOIncStep : horizontal increasing step for odd pixel (4 bit)
 * @param     [in] lscHEIncstep : horizontal increasing step for even pixel (4 bit)
 * @param     [in] lscVOIncStep : vertical increasing step for odd pixel (4 bit)
 * @param     [in] lscVEIncStep : vertical increasing step for even pixel (4 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-6-24
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
HAL_CdspLscWinIncStepSet(
	UINT8 lscHOIncStep,
	UINT8 lscHEIncstep,
	UINT8 lscVOIncStep,
	UINT8 lscVEIncStep
)
{
	XBYTE[REG_Cdsp_LscLsxmoinc]= (lscHOIncStep&0x0f) | ((lscHEIncstep&0x0f)<<4);
	XBYTE[REG_Cdsp_LscLsymoinc]= (lscVOIncStep&0x0f) | ((lscVEIncStep&0x0f)<<4);

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspLscWinSizeSet(UINT8 LSChsize)
 * @brief     Set the Lens compensation step parameters
 * @param     [in] lscHSize : horizontal size  (8 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
HAL_CdspLscWinSizeSet(
	UINT8 lscHSize
)
{
     XBYTE[REG_Cdsp_LscLstblhsize]= lscHSize;
	 XBYTE[REG_Dram_LensGain_HSize]= lscHSize;

	 return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspLscResolution(UINT8 lscRes)
 * @brief     Set resolution of LSC
 * @param     [in] lscRes : 
 *							0: 1.9 bits (RGB 3 channel mode)		
 *							1: 2.8 bits (RGB 3 channel mode)
 *							2: 3.7 bits (RGB 3 channel mode)
 *							0: 1.(7+2) bits (RGB 4 channel mode)		
 *							1: 2.(6+2) bits (RGB 4 channel mode)
 *							2: 3.(5+2) bits (RGB 4 channel mode)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
HAL_CdspLscResolution(
	UINT8 lscRes
)
{
     XBYTE[REG_Cdsp_LscLsbitmode]= (lscRes&0x03);
	 return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspLscWinOffsetSet(UINT8 lscHOff, lscHOff,UINT8 lscVOff)
 * @brief     Set offset of LSC window
 * @param     [in] lscHOff : horizontal offset (8 bit)
 * @param     [in] lscVOff : vertical offset (8 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
HAL_CdspLscWinOffsetSet(
	UINT8 lscHOff,
	UINT8 lscVOff
)
{
     XBYTE[REG_Cdsp_LscLsxmoffset]= lscHOff; 
     XBYTE[REG_Cdsp_LscLsymoffset]= lscVOff;
	 return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspLscEnSet(UINT8 lscChMode, lscChMode,UINT8 lscEn)
 * @brief     Set mode and enable or disable LSC function
 * @param     [in] lscChMode : channel mode selection (1 bit)
 *					0 : RGB 3 channels
 *					1 : R Gr B Gb 4 channels
 * @param     [in] lscEn : 
 *					0: disable
 *					1: enable
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspLscEnSet(
	UINT8 lscEn
)
{
	if (lscEn==0){
		XBYTE[ REG_Cdsp_LscChmode ] &= 0xef;
		//HAL_CdspRstBlock(CDSPRstEDGE_LSC);
		//printf("LSC OFF\n");
	}
	else{
		XBYTE[ REG_Cdsp_LscChmode ] |= 0x10;
	}
	
   //XBYTE[ REG_Cdsp_LscChmode ] = (XBYTE[ REG_Cdsp_LscChmode ]& ~0x10) | ((lscEn&0x01)<<4) ;

   return HALK_SUCCESS; 
}


UINT8
HAL_CdspLscEnGet(
	void
)
{
   return ( XBYTE[ REG_Cdsp_LscChmode ]>>4);
}

/**
 * @fn        UINT8 HAL_CdspLscModeSet(UINT8 lscChMode)
 * @brief     HAL_CdspLscModeSet
 * @param     [in] lscChMode : 
 *								0: R/G/B three channel 
 *								1: R/Gr/B/Gb four channel  
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspLscModeSet(
	UINT8 lscChMode
)
{
   XBYTE[ REG_Cdsp_LscChmode ] = (XBYTE[ REG_Cdsp_LscChmode ]& ~0x01) | (lscChMode&0x01);

   return HALK_SUCCESS; 
}


/**
 * @fn        UINT8 HAL_CdspLscWinAddrset(UINT32 lscWinAddr)
 * @brief     Set store address of LSC window parameters in DRAM
 * @param     [in] lscWinAddr : store address of LSC parameters  (25 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_CdspLscWinAddrset(
	UINT32 lscWinAddr
)
{
		//	printf("HAL_CdspAEwinAddrset AEWinaddr =0x%lx \n",AEWinaddr);
		XBYTE[ REG_Dram_LensGain_StAddr_B0 ] = READ8( lscWinAddr, 3 );
		XBYTE[ REG_Dram_LensGain_StAddr_B1 ] = READ8( lscWinAddr, 2 );
		XBYTE[ REG_Dram_LensGain_StAddr_B2 ] = READ8( lscWinAddr, 1 );
		XBYTE[ REG_Dram_LensGain_StAddr_B3 ] = READ8( lscWinAddr, 0 );
#if 0
		printf("XBYTE[REG_Cdsp_Lscvalue_Byte0]=0x%bx \n",XBYTE[REG_Cdsp_Lscvalue_Byte0]);
		printf("XBYTE[REG_Cdsp_Lscvalue_Byte1]=0x%bx \n",XBYTE[REG_Cdsp_Lscvalue_Byte1]);
		printf("XBYTE[REG_Cdsp_Lscvalue_Byte2]=0x%bx \n",XBYTE[REG_Cdsp_Lscvalue_Byte2]);
		printf("XBYTE[REG_Cdsp_Lscvalue_Byte3]=0x%bx \n",XBYTE[REG_Cdsp_Lscvalue_Byte3]);
#endif
	
		return HALK_SUCCESS;

}


/**
 * @fn        UINT8 HAL_CdspYuvFilterBufSet(UINT8 yBufEn)
 * @brief     HAL_CdspYuvFilterBufSet
 * @param     [in] yBufEn : 0:Preview mode (1 bit)
 *\n						1:Still mode
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-6-29
 * @todo      N/A
 * @bug       N/A
*/

void
HAL_CdspYuvFilterBufSet(
	UINT8 yBufEn
)
{
	XBYTE[ REG_Cdsp_EdgeEn ] &= 0xef;
	XBYTE[ REG_Cdsp_EdgeEn ] |= (( yBufEn & 0x01 ) << 4 ) ;
}


/**
 * @fn        UINT8 HAL_CdspEdgeCoringMedianEnSet(UINT8 yEdgeEn, UINT8 lEdgeEn, UINT8 yCorEn, UINT8 uvLpfEn, UINT8 yBufEn)
 * @brief     set Edge/UV LPF/Y coring enable  function
 * @param     [in] yEdgeEn : Y edge function enable (Still Mode Only) (1 bit)
 * @param     [in] lEdgeEn : L Edge function enable (1 bit)
 * @param     [in] yCorEn : Y Coring function enable (Still Mode Only)(1 bit)
 * @param     [in] uvMedianEn : UV median function enable (1 bit)
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
/*
void
HAL_CdspEdgeCoringMedianEnSet(
	UINT8 yEdgeEn,
	UINT8 lEdgeEn,
	UINT8 yCorEn,
	UINT8 uvMedianEn
)
{
	XBYTE[ REG_Cdsp_EdgeEn ] &= 0xf0;
	XBYTE[ REG_Cdsp_EdgeEn ] |= ( ( uvMedianEn & 0x01 ) << 3 ) | ( ( yCorEn & 0x01 ) << 2 ) | ( ( lEdgeEn & 0x01 ) << 1 ) | ( yEdgeEn & 0x01 );
}
*/

/**
 * @fn        UINT8 HAL_CdspYEdgeEnSet(UINT8 yEdgeEn)
 * @brief     set Edge/UV LPF/Y coring enable  function
 * @param     [in] yEdgeEn : Y edge function enable (Still Mode Only) (1 bit)
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void 
HAL_CdspYEdgeEnSet(
	UINT8 yEdgeEn
)
{
	XBYTE[ REG_Cdsp_EdgeEn ] &= 0xfe;
	XBYTE[ REG_Cdsp_EdgeEn ] |= (yEdgeEn &0x01);
}

/**
 * @fn        UINT8 HAL_CdspLEdgeEnSet(UINT8 lEdgeEn)
 * @brief     set Edge/UV LPF/Y coring enable  function
 * @param     [in] lEdgeEn : L Edge function enable (1 bit)
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void 
HAL_CdspLEdgeEnSet(
	UINT8 lEdgeEn
)
{
	XBYTE[ REG_Cdsp_EdgeEn ] &= 0xfd;
	XBYTE[ REG_Cdsp_EdgeEn ] |= ((lEdgeEn &0x01)<<1);
}

/**
 * @fn        UINT8 HAL_CdspYCoringEnSet(UINT8 yCoringEn)
 * @brief     set Y coring enable function
 * @param     [in] yCoringEn : Y Coring function enable (Still Mode Only)(1 bit)
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void 
HAL_CdspYCoringEnSet(
	UINT8 yCoringEn
)
{
	XBYTE[ REG_Cdsp_EdgeEn ] &= 0xfb;
	XBYTE[ REG_Cdsp_EdgeEn ] |= ((yCoringEn &0x01)<<2);
}

/**
 * @fn        UINT8 HAL_CdspUVMedianEnSet(UINT8 yEdgeEn)
 * @brief     set UV Median enable  function
 * @param     [in] uvMedianEn : UV median function enable (1 bit)
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void 
HAL_CdspUVMedianEnSet(
	UINT8 uvMedianEn
)
{
	XBYTE[ REG_Cdsp_EdgeEn ] &= 0xf7;
	XBYTE[ REG_Cdsp_EdgeEn ] |= ((uvMedianEn &0x01)<<3);
}

/**
 * @fn        void HAL_CdspMedianModeSet(UINT8 medianMode)
 * @brief     Set median filter mode
 * @param     [in] medianMode : 0 5x1 median mode (preview mode only)
 *\n							1 5x1 LPF mode (preview mode only)
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-10-8
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_CdspMedianModeSet(
	UINT8 medianMode
)
{
	XBYTE[REG_Cdsp_Medmode]=medianMode;
}
/**
 * @fn        UINT8 HAL_CdspEdgeSuppressionMir(UINT8 edgeMirHLEn, UINT8 edgeMirHREn, UINT8 edgeMirVTEn, UINT8 edgeMirVDEn)
 * @brief     set Edge mirror function
 * @param     [in] edgeMirHLEn : Edge horizontal left mirror 2 pixels (1 bit)
 * @param     [in] edgeMirHREn : Edge horizontal right mirror 2 pixels (1 bit)
 * @param     [in] edgeMirVTEn : Edge vertical top mirror 2 pixels (1 bit)
 * @param     [in] edgeMirVDEn : Edge vertical down mirror 2 pixels (1 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspEdgeSuppressionMir(
	UINT8 edgeMirHLEn,
	UINT8 edgeMirHREn,
	UINT8 edgeMirVTEn,
	UINT8 edgeMirVDEn
)
{
	XBYTE[ REG_Cdsp_EsmirEn ] = ( ( edgeMirVDEn & 0x01 ) << 3 ) | ( ( edgeMirVTEn & 0x01 ) << 2 ) | ( ( edgeMirHREn & 0x01 ) << 1 ) | ( edgeMirHLEn & 0x01 );

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspYCoringCoefficientSet(UINT8 yCor0, UINT8 yCor1, UINT8 yCor2, UINT8 yCor3, UINT8 yCor4, UINT8 yCor5, UINT8 yCor6, UINT8 yCor7, UINT8 yCor8, UINT8 yCor9, UINT8 yCor10, UINT8 yCor11, UINT8 yCor12, UINT8 yCor13, UINT8 yCor14, UINT8 yCor15, UINT8 yWeighting)
 * @brief     Set YCoring Coefficient value
 * @param     [in] yCor0 : Y Coring Coefficient(8 bit)
 * @param     [in] yCor1 : Y Coring Coefficient(8 bit)
 * @param     [in] yCor2 : Y Coring Coefficient(8 bit)
 * @param     [in] yCor3 : Y Coring Coefficient(8 bit)
 * @param     [in] yCor4 : Y Coring Coefficient(8 bit)
 * @param     [in] yCor5 : Y Coring Coefficient(8 bit)
 * @param     [in] yCor6 : Y Coring Coefficient(8 bit)
 * @param     [in] yCor7 : Y Coring Coefficient(8 bit)
 * @param     [in] yCor8 : Y Coring Coefficient(8 bit)
 * @param     [in] yCor9 : Y Coring Coefficient(8 bit)
 * @param     [in] yCor10 : Y Coring Coefficient(8 bit)
 * @param     [in] yCor11 : Y Coring Coefficient(8 bit)
 * @param     [in] yCor12 : Y Coring Coefficient(8 bit)
 * @param     [in] yCor13 : Y Coring Coefficient(8 bit)
 * @param     [in] yCor14 : Y Coring Coefficient(8 bit)
 * @param     [in] yCor15 : Y Coring Coefficient(8 bit)
 * @param     [in] yWeighting : Y Weighting (3 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspYCoringCoefficientSet(
	UINT8 *par
)
{
	UINT8 i;
	for (i=0;i<17;i++){
		XBYTE[REG_Cdsp_Ycoring0+i]=*(par+i);
		//printf("cor[0x%x]=0x%bx\n",(REG_Cdsp_Ycoring0+i),XBYTE[REG_Cdsp_Ycoring0+i]);
	}

	return HALK_SUCCESS;
}



	
/**
 * @fn        UINT8 HAL_CdspEdgeYHDivSet(UINT8 yhDiv)
 * @brief     HAL_CdspEdgeYHDivSet
 * @param     [in] yhDiv : Edge value scale		0~11 : 1/(2^yhdiv)(4 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspEdgeYHDivSet(
	UINT8 yhDiv
)
{
	XBYTE[ REG_Cdsp_Yhdiv ] = yhDiv & 0x0f;
	
	return HALK_SUCCESS;
}


/**
 * @fn        UINT8 HAL_CdspEdgeYHClampSet(UINT8 yhtClamp)
 * @brief     HAL_CdspEdgeYHClampSet
 * @param     [in] yhtClamp : Edge value clamp threshold(8 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspEdgeYHClampSet(
	UINT8 yhtClamp

)
{
	XBYTE[ REG_Cdsp_Yhtclamp ] = yhtClamp;
	
	return HALK_SUCCESS;
}



	
/**
 * @fn        UINT8 HAL_CdspEdgeGainSet(UINT8 gain0, UINT8 gain1)
 * @brief     Set CDSP Edge Gain
 * @param     [in] gain0 : Edge enhancement low gain (2.6bits)
 * @param     [in] gain1 : Edge enhancement high gain (2.6bits)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspEdgeGainSet(
	UINT8 gain0,
	UINT8 gain1
)
{
	XBYTE[ REG_Cdsp_ygain0 ] = gain0;
	XBYTE[ REG_Cdsp_ygain1 ] = gain1;
	
	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspEdgeThrSet(UINT8 yhtLowThr, UINT8 yhtHighThr)
 * @brief     Set CDSP Edge threshold and bandwidth value
 * @param     [in] yhtLowThr : Edge value low threshold for low gain (8 bit)
 * @param     [in] yhtHighThr : Edge value high threshold for high gain (8 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspEdgeThrSet(
	UINT8 yhtLowThr,
	UINT8 yhtHighThr
)
{
	XBYTE[ REG_Cdsp_yhtlowthr ] = yhtLowThr;
	XBYTE[ REG_Cdsp_yhthighthr ] = yhtHighThr;

	
	return HALK_SUCCESS;
}


/**
 * @fn        UINT8 HAL_CdspEdgeBwSet(UINT8 yhtBw)
 * @brief     set edge gain transition bandwidth 
 * @param     [in] yhtBw :
 *							[2:0] Edge gain transition bandwidth for low gain
 *							[6:4] Edge gain transition bandwidth for high gain 
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
HAL_CdspEdgeBwSet(
	UINT8 yhtBw
)
{
	XBYTE[ REG_Cdsp_yhtbw ] = yhtBw;
	return HALK_SUCCESS;
}

	
/**
 * @fn        UINT8 HAL_CdspEdgeFilterSet(UINT8 ftr0, UINT8 ftr5, UINT8 ftr4, UINT8 ftr3, UINT8 ftr2, UINT8 ftr1)
 * @brief     Set CDSP Edge Filter
 * @param     [in] ftr0 :HPF mask coefficient of edge enhancement (2's complement, 1s7b)
 *						5x5 matrix for still mode
 *							Ftr5 Ftr4 Ftr3 Ftr4 Ftr5
 *							Ftr4 Ftr2 Ftr1 Ftr2 Ftr4
 *							Ftr3 Ftr1 Ftr0 Ftr1 Ftr3
 *							Ftr4 Ftr2 Ftr1 Ftr2 Ftr4
 *							Ftr5 Ftr4 Ftr3 Ftr4 Ftr5
 *						3x3 matrix for preview mode
 *							Ftr2 Ftr1 Ftr2
 *							Ftr1 Ftr0 Ftr1
 *							Ftr2 Ftr1 Ftr2
 * @param     [in] ftr5 : HPF mask coefficient of edge enhancement (sign+3bits)
 *							0 : 0
 *							1 : 1
 *							2 : 2
 *							3 : 3
 *							4 : 4
 *							9 : -1
 *							A : -2
 *							B : -3
 *							C : -4
 * @param     [in] ftr4 : the same as ftr5
 * @param     [in] ftr3 : the same as ftr5
 * @param     [in] ftr2 : HPF mask coefficient of edge enhancement  (2's complement, 1s7b)
 * @param     [in] ftr1 : HPF mask coefficient of edge enhancement  (2's complement, 1s7b)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspEdgeFilterSet(
	UINT8 ftr0,
	UINT8 ftr5,
	UINT8 ftr4,
	UINT8 ftr3,
	UINT8 ftr2,
	UINT8 ftr1
)
{
	XBYTE[ REG_Cdsp_ftr0 ] = ftr0;
	XBYTE[ REG_Cdsp_ftr5 ] = ( ftr5 & 0x0f );
	XBYTE[ REG_Cdsp_ftr4 ] = ( ftr4 & 0x0f );
	XBYTE[ REG_Cdsp_ftr3 ] = ( ftr3 & 0x0f );
	XBYTE[ REG_Cdsp_ftr2 ] = ( ftr2 );
	XBYTE[ REG_Cdsp_ftr1 ] = ( ftr1 );

	return HALK_SUCCESS;
}

/**
 * @fn        void HAL_CdspEffectYuvNormalSave(void)
 * @brief     Save YUV saturation/hue parameters
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_CdspEffectYuvNormalSave(
	void
)
{
	halCdspReg21b0 = XBYTE[ 0x21b0 ];
	halCdspReg21b4 = XBYTE[ 0x21b4 ];
	halCdspReg21b5 = XBYTE[ 0x21b5 ];
	halCdspReg21b6 = XBYTE[ 0x21b6 ];
	halCdspReg21b7 = XBYTE[ 0x21b7 ];
	halCdspReg21b8 = XBYTE[ 0x21b8 ];
	halCdspReg21b9 = XBYTE[ 0x21b9 ];
	halCdspReg21ba = XBYTE[ 0x21ba ];
	halCdspReg21bc = XBYTE[ 0x21bc ];
	halCdspReg21bd = XBYTE[ 0x21bd ];

	return;
}

/**
 * @fn        void HAL_CdspEffectYuvNormalResotre(void)
 * @brief     restore YUV saturation/hue parameters 
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_CdspEffectYuvNormalResotre(
	void
)
{
	XBYTE[ 0x21b4 ] = halCdspReg21b4;
	XBYTE[ 0x21b5 ] = halCdspReg21b5;
	XBYTE[ 0x21b6 ] = halCdspReg21b6;
	XBYTE[ 0x21b7 ] = halCdspReg21b7;
	XBYTE[ 0x21b8 ] = halCdspReg21b8;
	XBYTE[ 0x21b9 ] = halCdspReg21b9;
	XBYTE[ 0x21ba ] = halCdspReg21ba;
	XBYTE[ 0x21bc ] = halCdspReg21bc;
	XBYTE[ 0x21bd ] = halCdspReg21bd;
	XBYTE[ 0x21b0 ] = halCdspReg21b0;

	return;
}


/**
 * @fn        UINT8 HAL_CdspDarkTDSizeSet(UINT16 hsize)
 * @brief     Set horizontal size for dark-sub/Temporal Denoise
 * @param     [in] hsize : horiztontal size
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_CdspDarkTDSizeSet(
	UINT16 hSize
)
{
	XBYTE[0x229A] = (hSize) & 0xFF;			
	XBYTE[0x229B] = (hSize>>8) & 0xFF;    
	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_IntrCdspDarkTDSizeSet(UINT16 hsize)
 * @brief     Set horizontal size for dark-sub/Temporal Denoise
 * @param     [in] hsize : horiztontal size
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_IntrCdspDarkTDSizeSet(
	UINT16 hSize
)
{
	XBYTE[0x229A] = (hSize) & 0xFF;			
	XBYTE[0x229B] = (hSize>>8) & 0xFF;    
	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspDarkCfg(UINT8 mode, UINT8 src)
 * @brief     Set dark-sub mode
 * @param     [in] mode : (1 bit)
 *							0: dram - input image
 *							1: input image - dram
 * @param     [in] src : (1 bit)
 *							0: Dark substraction after gamma
 *							1: Dark substraction in rawmux
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_CdspDarkCfg(
	UINT8 src,
	UINT8 mode
)
{
	XBYTE[REG_Cdsp_3Ddarksubmode]= (src&0x01) | ((mode&0x01)<<4);
	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspDarkEnSet(UINT8 en)
 * @brief     Enable temporal denoise
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
HAL_CdspDarkEnSet(
	UINT8 en
)
{
 	XBYTE[ REG_Cdsp_3Dtden ] &= 0xfd;
	XBYTE[ REG_Cdsp_3Dtden ] |= ((en&0x01)<<1);

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspStillBpCfg(UINT8 mode, UINT8 sync, UINT16 hSize)
 * @brief     configure still bad pixel compensation
 * @param     [in] mode :  
 *						0: CPU control mode
 *						1: Automatic mode
 * @param     [in] sync :
 *						0: Manual mode						
 *						1: Sync to cdsp raw data mode 
 * @param     [in] hSize : horizontal size
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_CdspStillBpCfg(
	UINT8 mode,
	UINT8 sync,
	UINT8 rip,
	UINT16 hSize
)
{	
	XBYTE[REG_Moe_OperationMode] = (mode&0x01) | ((sync&0x01)<<4) | ((rip&0x03)<<6);   // [0]cpu mode or command queue mode 
	                                                  // [4]bad pixel syncmode	

	//XBYTE[0x2e21] = 0x00;   // 

	XBYTE[REG_Moe_CmdROIWidth0] = LowByte(hSize);	
	XBYTE[REG_Moe_CmdROIWidth1] = HighByte(hSize);

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspStillBpEnSet(UINT8 en)
 * @brief     enable still bad pixel 
 * @param     [in] en : 
 *						0: disable
 *						1: enablue
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_CdspStillBpEnSet(
	UINT8 en
)
{
	XBYTE[REG_Moe_CmdOpt] &= 0x7f;
	XBYTE[REG_Moe_CmdOpt] |= ((en&0x01)<<7);	// bad pixel function
	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspStillBpTblAddrSet(UINT32 bpTblAddr)
 * @brief     Set address of bap pixel table for still capture
 * @param     [in] bpTblAddr : bad pixel table address
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_CdspStillBpTblAddrSet(
	UINT32 bpTblAddr
)
{	
	XBYTE[REG_Moe_CmdSrc10] = READ8( bpTblAddr, 3 );  
	XBYTE[REG_Moe_CmdSrc11] = READ8( bpTblAddr, 2 );
	XBYTE[REG_Moe_CmdSrc12] = READ8( bpTblAddr, 1 );
	XBYTE[REG_Moe_CmdSrc13] = READ8( bpTblAddr, 0 );

	//printf("2E30=%bx\n",XBYTE[0x2e30]);	
	//printf("2E31=%bx\n",XBYTE[0x2e31]);	
	//printf("2E32=%bx\n",XBYTE[0x2e32]);	
	//printf("2E33=%bx\n",XBYTE[0x2e33]);	
	
	return HALK_SUCCESS;
}


/**
 * @fn        UINT8 HAL_CdspStillBpRAWAddrSet(UINT32 bpRawAddr)
 * @brief     Set raw data address for bad pixel compensation
 * @param     [in] bpRawAddr : raw data address
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
HAL_CdspStillBpRAWAddrSet(
	UINT32 bpRawAddr
)
{
	XBYTE[REG_Moe_CmdSrc20] = READ8( bpRawAddr, 3 );  
	XBYTE[REG_Moe_CmdSrc21] = READ8( bpRawAddr, 2 );
	XBYTE[REG_Moe_CmdSrc22] = READ8( bpRawAddr, 1 );
	XBYTE[REG_Moe_CmdSrc23] = READ8( bpRawAddr, 0 );
	
	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspStillBpTrigger(UINT8 bpTrig)
 * @brief     Trigger bad pixel compensation
 * @param     [in] bpTrig : 
 *							1: trigger
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/

UINT8 
HAL_CdspStillBpTrigger(
	UINT8 bpTrig
)
{
	if(XBYTE[REG_Moe_ModuleControl]){
		XBYTE[REG_Moe_ModuleControl]=0;//wangjg++ enable moe
	}
	
    XBYTE[REG_Moe_CmdTrigger]= (bpTrig&0x01);
	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspPvBpTblAddrSet(UINT32 pvBPTblAddr)
 * @brief     Set address of bad pixel table for preview
 * @param     [in] pvBPTblAddr : preview bad pixel table address
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
HAL_CdspPvBpTblAddrSet(
	UINT32 pvBPTblAddr
)
{
	XBYTE[REG_Cdsp_badbaseaddr_byte0] = READ8( pvBPTblAddr, 3 );  
	XBYTE[REG_Cdsp_badbaseaddr_byte1] = READ8( pvBPTblAddr, 2 );
	XBYTE[REG_Cdsp_badbaseaddr_byte2] = READ8( pvBPTblAddr, 1 );
	XBYTE[REG_Cdsp_badbaseaddr_byte3] = READ8( pvBPTblAddr, 0 );
	
	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspPvBpEnSet(UINT8 pvBpEn)
 * @brief     enable preview bad pixel compensation
 * @param     [in] pvBpEn :
 *							0: disable
 *							1: enable	
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
HAL_CdspPvBpEnSet(
	UINT8 pvBpEn
)
{
	XBYTE[REG_Cdsp_badpixen] &= 0xfe;
    XBYTE[REG_Cdsp_badpixen] |= (pvBpEn & 0x01);
	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspWDRTblnit(UINT8* pWdrData)
 * @brief     Set WDR table into SRAM
 * @param     [in] pWdrData : the address of WDR table 
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspWDRTblnit(
	UINT8* pWdrData
)
{
	UINT16 i;

	XBYTE[ REG_Cdsp_macroSel ] = 0x05;
	XBYTE[ REG_Cdsp_switchclk ] = 0x05;
	XBYTE[ REG_Cdsp_macropagsel ] = 0x00;
	
	//printf("pwdrdata[0]=0x%bx \n",pwdrdata[0]);	
	//printf("pwdrdata[1]=0x%bx \n",pwdrdata[1]);
	//printf("pwdrdata[2]=0x%bx \n",pwdrdata[2]);
	//printf("pwdrdata[3]=0x%bx \n",pwdrdata[3]);

	for( i = 0; i < 256; i ++ ) {
		XBYTE[0x2b00 + i] = pWdrData[i];
		//printf("*wdrdata[%d]=0x%bx \n", i, wdrdata[i]);
		
		while ( XBYTE[0x2b00 + i] != pWdrData[i]){
			XBYTE[0x2b00 + i] = pWdrData[i];
			//printf("Re-Write SRAM \n");
		}
	}

	XBYTE[ REG_Cdsp_macroSel ] = 0x00;
	XBYTE[ REG_Cdsp_switchclk ] = 0x00;
	XBYTE[ REG_Cdsp_macropagsel ] = 0x00;

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspWDREnSet(UINT8 en)
 * @brief     Enable/Disable WDR 
 * @param     [in] en : 1 : enable 
 *\n					0 : disable
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-6-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspWDREnSet(
	UINT8 en
)
{
	XBYTE[REG_Cdsp_WDRen] &= 0xfe;
	XBYTE[REG_Cdsp_WDRen] |= (en&0x01);
	
	return HALK_SUCCESS;
}


/**
 * @fn        UINT8 HAL_CdspTDCfg(UINT8 rst, UINT8 sync, UINT8 zf)
 * @brief     configure temporal denoise
 * @param     [in] rst : reset temporal denoise
 * @param     [in] sync : 
 *						0: immediate update
 *						1: sync with VD
 *						2: sync with ZF update
 * @param     [in] zf :
 *						0: disable
 *						1: restart when zoom factor updata happens
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
HAL_CdspTDCfg(	
	UINT8 rst,
	UINT8 sync,
	UINT8 zf
)
{
	UINT8 val;
	val = XBYTE[ REG_Cdsp_3Dtden ];

	val &= (~0x78);	/*clear bit 3~6 */
	XBYTE[ REG_Cdsp_3Dtden ] = (val | ( ((rst&0x01)<<3) | ((sync&0x03)<<4) | ((zf&0x01)<<6)));

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspTDEnSet(UINT8 en)
 * @brief     Enable temporal denoise
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
HAL_CdspTDEnSet(
	UINT8 en
)
{
 	XBYTE[ REG_Cdsp_3Dtden ] &= 0xfe;
	XBYTE[ REG_Cdsp_3Dtden ] |= (en&0x01);

	/*
	if ((en&0x01)==0x01){
		XBYTE[ REG_Cdsp_3Dtden ] |= 0x01;	
	}
	else{
		XBYTE[ REG_Cdsp_3Dtden ] &= (~0x01);	
	}
	*/
	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_IntrCdspTDEnSet(UINT8 en)
 * @brief     Enable temporal denoise
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
HAL_IntrCdspTDEnSet(
	UINT8 en
)
{
 	XBYTE[ REG_Cdsp_3Dtden ] &= 0xfe;
	XBYTE[ REG_Cdsp_3Dtden ] |= (en&0x01);

	/*
	if ((en&0x01)==0x01){
		XBYTE[ REG_Cdsp_3Dtden ] |= 0x01;	
	}
	else{
		XBYTE[ REG_Cdsp_3Dtden ] &= (~0x01);	
	}
	*/
	return HALK_SUCCESS;
}


/**
 * @fn        UINT8 HAL_CdspHueCorrectionEnSet(UINT8 enable)
 * @brief     Enable/Disable hue correction
 * @param     [in] enable : 
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
HAL_CdspHueCorrectionEnSet(
	UINT8 enable
)
{
	XBYTE[ REG_Cdsp_huecorren ] =  (enable & 0x01);

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspHueCorrectionXSet(UINT16 angleX, UINT8 n1x, UINT8 n2x, UINT8 n3x, UINT8 n4x, UINT8 shx, UINT8 ssx)
 * @brief     set  1st hue correction angle parameter
 * @param     [in] angleX : Hue angle of tuning color X (9 bit)
 * @param     [in] n1x : Hue rotate of  X (3 bit)
 * @param     [in] n2x : Hue span of X (3 bit)
 * @param     [in] n3x : saturation gain of X (3 bit)
 * @param     [in] n4x : saturation span of X (3 bit)
 * @param     [in] shx : Hue rotate direction bit of X (1 bit)
 * @param     [in] ssx : saturation gain direction bit of X (1 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspHueCorrectionXSet(
	UINT16 angleX,
	UINT8 n1x,
	UINT8 n2x,
	UINT8 n3x,
	UINT8 n4x,
	UINT8 shx,
	UINT8 ssx
)
{
	XBYTE[ REG_Cdsp_hcangx_low ] = LowByte( angleX );
	XBYTE[ REG_Cdsp_hcangx_high ] = HighByte( angleX );
	XBYTE[ REG_Cdsp_hcn12x ] = ( UINT8 ) ( ( n2x & 0x07 ) << 4 | ( n1x & 0x07 ) );
	XBYTE[ REG_Cdsp_hcn34x ] = ( UINT8 ) ( ( n4x & 0x07 ) << 4 | ( n3x & 0x07 ) );
	XBYTE[ REG_Cdsp_hcshx ] = ( UINT8 ) ( ssx & 0x01 ) << 4 | ( shx & 0x01 );

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspHueCorrectionYSet(UINT16 angleY, UINT8 n1y, UINT8 n2y, UINT8 n3y, UINT8 n4y, UINT8 shy, UINT8 ssy)
 * @brief     set  2nd hue correction angle parameter
 * @param     [in] angleY : Hue angle of tuning color Y (9 bit)
 * @param     [in] n1y : Hue rotate of  Y (3 bit)
 * @param     [in] n2y : Hue span of Y (3 bit)
 * @param     [in] n3y : saturation gain of Y (3 bit)
 * @param     [in] n4y : saturation span of Y (3 bit)
 * @param     [in] shy : Hue rotate direction bit of Y (1 bit)
 * @param     [in] ssy : saturation gain direction bit of Y (1 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspHueCorrectionYSet(
	UINT16 angleY,
	UINT8 n1y,
	UINT8 n2y,
	UINT8 n3y,
	UINT8 n4y,
	UINT8 shy,
	UINT8 ssy
)
{
	XBYTE[ REG_Cdsp_hcangy_low ] = LowByte( angleY );
	XBYTE[ REG_Cdsp_hcangy_high ] = HighByte( angleY )&0x01;
	XBYTE[ REG_Cdsp_hcn12y ] = ( UINT8 ) ( ( n2y & 0x07 ) << 4 | ( n1y & 0x07 ) );
	XBYTE[ REG_Cdsp_hcn34y ] = ( UINT8 ) ( ( n4y & 0x07 ) << 4 | ( n3y & 0x07 ) );
	XBYTE[ REG_Cdsp_hcshy ] = ( UINT8 ) ( ssy & 0x01 ) << 4 | ( shy & 0x01 );

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_CdspHueCorrectionZSet(UINT16 angleZ, UINT8 n1z, UINT8 n2z, UINT8 n3z, UINT8 n4z, UINT8 shz, UINT8 ssz, UINT8 ythrz)
 * @brief     set  3rd hue correction angle parameter
 * @param     [in] angleZ : Hue angle of tuning color Z (9 bit)
 * @param     [in] n1z : Hue rotate of  Z (3 bit)
 * @param     [in] n2z : Hue span of Z (3 bit)
 * @param     [in] n3z : saturation gain of Z (3 bit)
 * @param     [in] n4z : saturation span of Z (3 bit)
 * @param     [in] shz : Hue rotate direction bit of Z (1 bit)
 * @param     [in] ssz : saturation gain direction bit of Z (1 bit)
 * @param     [in] ythrz : y threshold
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
HAL_CdspHueCorrectionZSet(
	UINT16 angleZ,  
	UINT8 n1z, 
	UINT8 n2z,
	UINT8 n3z, 
	UINT8 n4z,
	UINT8 shz,
	UINT8 ssz,
    UINT8 yThrz
)
{
    XBYTE[ REG_Cdsp_ythrz ] = yThrz;
	XBYTE[ REG_Cdsp_hcangz_low ] = LowByte( angleZ );
	XBYTE[ REG_Cdsp_hcangz_high ] = HighByte( angleZ )&0x01;
	XBYTE[ REG_Cdsp_hcn12z ] = ( UINT8 ) ( ( n2z & 0x07 ) << 4 | ( n1z & 0x07 ) );
	XBYTE[ REG_Cdsp_hcn34z ] = ( UINT8 ) ( ( n4z & 0x07 ) << 4 | ( n3z & 0x07 ) );
	XBYTE[ REG_Cdsp_hcshz ] = ( UINT8 ) ( ssz & 0x01 ) << 4 | ( shz & 0x01 );

	return HALK_SUCCESS;
}

/*****************************/
/*  unused functions         */
/*****************************/
#if 0
/**
 * @fn        void HAL_CdspEdgeYHClampGet(UINT8 *yhtClamp)
 * @brief     HAL_CdspEdgeYHClampGet
 * @param     [out] yhtClamp : Edge value clamp threshold(8 bit)
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_CdspEdgeYHClampGet(
	UINT8 *yhtClamp
)
{
	*yhtClamp = XBYTE[ REG_Cdsp_Yhtclamp ] ;
}

/**
 * @fn        void HAL_CdspEdgeYHDivGet(UINT8 *yhDiv)
 * @brief     HAL_CdspEdgeYHDivGet
 * @param     [out] yhDiv : Edge value scale	0~11 : 1/(2^yhdiv)(4 bit)
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_CdspEdgeYHDivGet(
	UINT8 *yhDiv
)
{
	*yhDiv = XBYTE[ REG_Cdsp_Yhdiv ] & 0x0f;	
}

/**
 * @fn        void HAL_CdspEdgeCoringMedianEnGet(UINT8 *yEdgeEn, UINT8 *lEdgeEn, UINT8 *yCorEn, UINT8 *uvLpfEn)
 * @brief     HAL_CdspEdgeCoringMedianEnGet
 * @param     [out] filterEn : [0] Y edge enable   [1] L edge enable
 *\n                           [2] Y coring enable [3] UV median enable
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_CdspEdgeCoringMedianEnGet(
	UINT8 *filterEn
)
{
	*filterEn =  XBYTE[ REG_Cdsp_EdgeEn ]& 0x0f;
}

/**
 * @fn        void HAL_CdspEdgeBwGet(UINT8 *yhtBw)
 * @brief     get edge gain transition bandwidth
 * @param     [out] yhtBw
 *							[2:0] Edge gain transition bandwidth for low gain
 *							[6:4] Edge gain transition bandwidth for high gain  
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_CdspEdgeBwGet(
	UINT8 *yhtBw
)
{
	*yhtBw = XBYTE[ REG_Cdsp_yhtbw ];
}

/**
 * @fn        void HAL_CdspEdgeGainGet(UINT8 gain0, UINT8 gain1)
 * @brief     HAL_CdspEdgeGainGet
 * @param     [out] gain0 : Edge enhancement low gain (2.6bits)
 * @param     [out] gain1 : Edge enhancement high gain (2.6bits)
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_CdspEdgeGainGet(
	UINT8 *gain0,
	UINT8 *gain1
)
{
	*gain0 = XBYTE[ REG_Cdsp_ygain0 ];
	*gain1 = XBYTE[ REG_Cdsp_ygain1 ];

}

/**
 * @fn        UINT8 HAL_CdspEdgeThrGet(UINT8 yhtLowThr, UINT8 yhtHighThr)
 * @brief     Set CDSP Edge threshold and bandwidth value
 * @param     [out] yhtLowThr : Edge value low threshold for low gain (8 bit)
 * @param     [out] yhtHighThr : Edge value high threshold for high gain (8 bit)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_CdspEdgeThrGet(
	UINT8 *yhtLowThr,
	UINT8 *yhtHighThr
)
{
	*yhtLowThr = XBYTE[ REG_Cdsp_yhtlowthr ];
	*yhtHighThr= XBYTE[ REG_Cdsp_yhthighthr ];

	
	return HALK_SUCCESS;
}

/**
 * @fn        void HAL_CdspEdgeFilterGet(UINT8 *ftr0, UINT8 *ftr5, UINT8 *ftr4, UINT8 *ftr3, UINT8 *ftr2, UINT8 *ftr1)
 * @brief     HAL_CdspEdgeFilterGet
 * @param     [out] ftr0 :HPF mask coefficient of edge enhancement (2's complement, 1s7b)
 *						5x5 matrix for still mode
 *							Ftr5 Ftr4 Ftr3 Ftr4 Ftr5
 *							Ftr4 Ftr2 Ftr1 Ftr2 Ftr4
 *							Ftr3 Ftr1 Ftr0 Ftr1 Ftr3
 *							Ftr4 Ftr2 Ftr1 Ftr2 Ftr4
 *							Ftr5 Ftr4 Ftr3 Ftr4 Ftr5
 *						3x3 matrix for preview mode
 *							Ftr2 Ftr1 Ftr2
 *							Ftr1 Ftr0 Ftr1
 *							Ftr2 Ftr1 Ftr2
 * @param     [out] ftr5 : HPF mask coefficient of edge enhancement (sign+3bits)
 *							0 : 0
 *							1 : 1
 *							2 : 2
 *							3 : 3
 *							4 : 4
 *							9 : -1
 *							A : -2
 *							B : -3
 *							C : -4
 * @param     [out] ftr4 : the same as ftr5
 * @param     [out] ftr3 : the same as ftr5
 * @param     [out] ftr2 : HPF mask coefficient of edge enhancement  (2's complement, 1s7b)
 * @param     [out] ftr1 : HPF mask coefficient of edge enhancement  (2's complement, 1s7b)
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_CdspEdgeFilterGet(
	UINT8 *ftr0,
	UINT8 *ftr5,
	UINT8 *ftr4,
	UINT8 *ftr3,
	UINT8 *ftr2,
	UINT8 *ftr1
)
{
	*ftr0 = XBYTE[ REG_Cdsp_ftr0 ];
	*ftr5 = (XBYTE[ REG_Cdsp_ftr5 ] & 0x0f);
	*ftr4 = (XBYTE[ REG_Cdsp_ftr4 ] & 0x0f);
	*ftr3 = (XBYTE[ REG_Cdsp_ftr3 ] & 0x0f);
	*ftr2 = XBYTE[ REG_Cdsp_ftr2 ];
	*ftr1 = XBYTE[ REG_Cdsp_ftr1 ];
}

#endif

