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

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "asicreg.h"
#include "ae_api.h"
#include "ae_def.h"
#include "ae_debug.h"
#include "front_logi.h"
#include "initio.h"
#include "sp1k_cdsp_api.h"
#include "sp1k_calibration_api.h"
#include "hal.h"
#include "hal_cdsp.h"
#include "dbg_def.h"
#include "hal_front.h"
#include "hal_global.h"
#include "reg_def.h"
#include "sp1k_ae_api.h"
#include "sp1k_front_api.h"
#include "gpio_api.h"
#include "hal_fd.h"
#include "awb_api.h"
#include "cdsp_task.h"
#include "icat_stub.h"
#include "sp1k_osd_api.h"
#include "os_api.h"
#include "af_api.h"


#if (AF_FIXTURE_EN == 1)

#if 0
AFstr afPara = {
	0,		/* afFlt2Type : Filter type 0 : (0, -1, 2, -1, 0) 1 : (1, -4, 6, -4, 1) */
	3,		/* afRgbMode : AF statistic data mode 0 : R   ,1 : G  , 2 : B  ,   3 : R+G+B (1 bit) */
	1,		/* afPgType : curve type : 0:slop 1/4, 1 : slop 1/2, 2 : custom gamma1 curve, 3 : custom gamma2 curve*/
	8,		/* afFlt1Wt */
	8,		/* afFlt2Wt */
	10,		/* afCorThr */
	0,		/* afCorVal */
};
#else
AFstr afPara = {
	1,		/* afFlt2Type */
	1,		/* afRgbMode */
	0,		/* afPgType */
	8,		/* afFlt1Wt */
	0,		/* afFlt2Wt */
	10,		/* afCorThr */
	0,		/* afCorVal */
};
#endif

UINT8 afEn;
UINT32 AFWinvalue;

/**************************************************************************
 *                           C O N S T A N T S
 **************************************************************************/
#define __FILE	__FILE_ID_LIB_AF_API__

/**************************************************************************
 *                              M A C R O S
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/**
 * @brief	Enable/Disable AF control
 * @param	en	: disable/enable, 0/1
 * @author	LinJieCheng
 * @todo
 * @since	2012-5-10
 */
void 
afWinEnSet(
	UINT8 en
)
{
	HAL_CdspAFWinEn(en,0,0,afPara.afFlt2Type,1);
	XBYTE[REG_Cdsp_evnt1] = 0x10;
	afEn = en;
}

/**
 * @brief	 Get Enable/Disable AF status
 * @param	NULL
 * @author	LinJieCheng
 * @todo
 * @return	AF Enable/Disable
 * @since	2012-5-10
 */
UINT8 
afWinEnGet(
	void
)
{
	return afEn;
}

/**
 * @brief	 Set AF Window Config
 * @param	frontHSize : sensor output h size
 * @param     frontVSize : sensor output v size
 * @param	cropHSize : roi h size
 * @param     cropVSize : roi v size
 * @author	LinJieCheng
 * @todo
 * @return	AF Enable/Disable
 * @since	2012-5-10
 */
void
afWinCfg(
	UINT16 frontHSize,
	UINT16 frontVSize,
	UINT16 cropHSize,
	UINT16 cropVSize
)
{
	
	UINT16 afHShift,afVShift;
	UINT16 afHStart,afHEnd,afVStart,afVEnd;

	/* AF filter 1/2 weight */
	HAL_CdspAFFilterWtSet(afPara.afFlt1Wt,afPara.afFlt2Wt);

	/* AF statistic data mode, AF pg type */
	HAL_CdspAFDataModeSet(afPara.afRgbMode,afPara.afPgType);

	/* AF coring threshold, value */
	HAL_CdspAFCoringSet(afPara.afCorThr,afPara.afCorVal);   // coring threshold and gain  

	#if 0
	afHShift = (frontHSize - cropHSize)/2; /* OV5647:(1280-1280)/2 */
	afVShift = (frontVSize - cropVSize)/2; /* OV5647:(960-960)/2 */
	
	afHStart = 7 + afHShift;	/* OV5647:7+0 */
	afHEnd = 16 + afHShift;		/* OV5647:16+0 */

	afVStart = 7 + afVShift;	/* OV5647:7+0 */
	afVEnd = 8 + afVShift;		/* OV5647:8+0 */

	HAL_CdspAFWinSizeSet(afHStart,cropHSize-afHEnd,afVStart,cropVSize-afVEnd);
	#else
	cropHSize = ((cropHSize >> 2) << 1);
	cropVSize = ((cropVSize >> 2) << 1);

	afHShift = (frontHSize - cropHSize)/2; /* OV5647:(1280-640)/2=320 */
	afVShift = (frontVSize - cropVSize)/2; /* OV5647:(960-480)/2=240 */

	afHStart = afHShift;
	afHEnd = afHStart+cropHSize;
	
	afVStart = afVShift;
	afVEnd = afVStart + cropVSize;

	HAL_CdspAFWinSizeSet(afHStart,afHEnd,afVStart,afVEnd);
	#endif
}

/**
 * @brief	 Get AF Value
 * @param	NULL
 * @author	LinJieCheng
 * @todo
 * @return	AF Value
 * @since	2012-5-10
 */
UINT32 
afWinValueGet(
	void
)
{
    return AFWinvalue;
}



#endif
