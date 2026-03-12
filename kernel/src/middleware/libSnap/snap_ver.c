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
 *	v01, 20111024 wangjg
 *		a. initialize version
 *	v02,20111118 dongdong.wang
 *		a. clean redundant code
 *	v03,20111124 fan.xiao
 *		a. Add snap 16:9 image
 *	v04,20120202 linjie.cheng
 *		a. fixed snap 16:9 & 4:3 sensor crop size small than display size Qv & Thb fail issue.
 *	v05,20120222 linjie.cheng
 *		a. fixed non-blocking dzoom for still & video flow
 *	v06,20120312 linjie.cheng
 *		a. fixed snap normal breakdown debug flow
 *      b. add snap bypass qv thb flow
 *	v07,20120314 linjie.cheng
 *		a. high speed burst mode add store first option
 *	v08,20120316 peng.tan
 *		a. add snap still image flow
 *	v09,20120321 kai.wang
 *	    a. fixed snap background data type error
 *	v0a,20120326 linjie.cheng
 *	    a. fixed high speed burst capture random crash
 *             (need wait Eof first and wait Sof to trigger bad pixel)
 *	v0b,20120326 linjie.cheng
 *	    a. disable some debug information (const full)
 *	v0c,20120727 rf.wang
 *	    a. add blocking zoom and nonblocking zoom
 *	v0d, 20130318 linjie.cheng
 *           a. add panorama function 
 *           b. adjust motion detect flow
 *	v0e, 20130326 linjie.cheng
 *           a. fix panorama flow
 *           b. add ctrl edge yuv src define
 *           c. add ctrl md yuv src define
 *           d. add ctrl 2nd path yuv size define
 *
 **************************************************************************/
 
/**
 * @file      snap_libver.c
 * @brief     snap library
 * @author    Will Chiu
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "Snap_libver.h"
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
 *				I N T E R N A L	  D A T A				    		      *
 **************************************************************************/
static UINT8 code verStr[] = "$sn0e"; 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/




/**************************************************************************
 *
 *  Function Name: snapLibVerGet
 *
 *  Purposes: Get current libaray's version.
 *
 *  Descriptions:
 *
 *  Arguments:
 *
 *  Returns:
 *
 *  See also:
 *
 **************************************************************************/
UINT8 *
libSnapVerGet(
	void
)
{
	return (verStr + 3);
}

