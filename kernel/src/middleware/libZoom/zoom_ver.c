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
 *
 *	ZO01, 20120801 LinJieCheng
 *		a. modify number of version
 *	ZO02, 20130318 linjie.cheng
 *           a. add panorama function 
 *           b. adjust motion detect flow
 *	ZO03, 20130326 linjie.cheng
 *           a. fix panorama flow
 *           b. add ctrl edge yuv src define
 *           c. add ctrl md yuv src define
 *           d. add ctrl 2nd path yuv size define
 *************************************************************************/
 
/**
 * @file      Zoom_ver.c
 * @brief     zoom middleware library version
 * @author    LinJieCheng
 * @since     2012-08-01
 * @date      2012-08-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h" 
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
static UINT8 code verStr[] = "$ZO03"; 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**
 * @fn        UINT8 libZoomVerGet(void)
 * @brief     Get current libaray's version
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
UINT8 *
libZoomVerGet(
	void
)
{
	return (verStr + 3);
}

