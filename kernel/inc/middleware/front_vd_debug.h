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
 * @file      front_vd_debug.h
 * @brief     Brief
 * @author    Author's name
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __FRONT_VD_DEBUG_H__
#define __FRONT_VD_DEBUG_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "dbg_def.h"
#include "dbg_file_list.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/* front vd system information & front vd time information switch */
#define VD_DEBUG_EN		0


/* front vd system information */
#if (VD_DEBUG_EN)
#define frontvdSysInfo printf
#endif
#ifndef frontvdSysInfo
#define frontvdSysInfo	/_printf
#endif

/* front vd time information */
#if (VD_DEBUG_EN)
#define frontvdTimeInfo HAL_GlobalTimerRead10us
#endif
#ifndef frontvdTimeInfo
#define _frontvdTimeInfo	/HAL_GlobalTimerRead10us
#define frontvdTimeInfo		/_frontvdTimeInfo
#endif


/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef struct _VD_AVERAGE_TIME{
	UINT32 vdAvgTime;
	UINT8 vdCnt_s;
	UINT32 timeTick_s;
	UINT8 vdCnt_e;
	UINT32 timeTick_e;
}VD_AVERAGE_TIME;

 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

#endif  /* __FRONT_VD_DEBUG_H__ */
