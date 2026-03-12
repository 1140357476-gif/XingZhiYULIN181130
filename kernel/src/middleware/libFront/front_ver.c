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
 *	fr01, 20111021 LinJieCheng
 *		a. modify number of version
 *	fr02, 20111107 LinJieCheng
 *		a. add HQ preview (sensor zoom Architecture)function
 *	fr03, 20111110 LinJieCheng
 *		a. add Dzoom non-blocking flow
 *              b. removed old Dzoom non-blocking flow
 *	fr04, 20111114 Fan.Xiao
 *		a. update libFrontFlash for out door dummy flash AE parameters ERROR                                                                  *
 *	fr05, 20111208 LinJieCheng
 *		a. add HighQualityDigitalZoomModeSet, CapabilityGet api
 *      fr06, 20120223 linjie.cheng
 *              a. add high quality sensor zoom flow
 *      fr07, 20120702 linjie.cheng
 *              a. add VD cnt check debug function
 *      fr08, 20120727 rf.wang
 *              a. add blcoking zoom and nonblocking zoom
 *************************************************************************/
	 /**
	  * @file	   Front_libver.c
	  * @brief	   version of front middleware
	  * @author    Will Chiu
	  * @since	   2010-01-01
	  * @date	   2010-01-01
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
 
static UINT8 code verStr[] = "$fr08";
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**
 * @fn        UINT8 frontLibVerGet(void)
 * @brief     Get current libaray's version
 * @param     NONE
 * @retval    return = version
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8 *
libFrontVerGet(
	void
)
{
	return (verStr + 3);
}

