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
 *	pv01, 20111021 LinJieCheng
 *		a. modify number of version
 *	pv02, 20111031 LinJieCheng
 *		a. add HQ preview function
 *	pv03, 20111107 LinJieCheng
 *		a. add HQ preview (sensor zoom Architecture)function
 *	pv04, 20111110 LinJieCheng
 *		a. add Dzoom non-blocking flow
 *              b. removed old Dzoom non-blocking flow 
  *	pv05, 20111124 fan.xiao
 *		a. modify for when snap 16:9 image , the preview will be 16:9 
 *  	pv06, 20111125 rf.wang
 *		a.fixed super HD pv change mode crash bug     
 *	pv07, 20111130 rf.wang
 *		a.fixed mantis bug[43746]  
 *		b.modify Compile warning  
 *	pv08, 20111208 LinJieCheng
 *		a.fixed HQ Dzoom flow
 *	pv09, 20111222 cxhe
 *		a.fixed pv buffer number=2 for pccam
 *	pv10, 20120207 rf.wang
 *		a.set 1st path YUV 4 buf mode when Dram size is 16M£¬ YUV 2 buf mode when Dram size is 8M
 *	pv11, 20120222 linjie.cheng
 *		a. fixed non-blocking dzoom for still & video flow
 *      pv12, 20120223 linjie.cheng
 *              a. add high quality sensor zoom flow
 *      pv13, 20120305 rf.wang
 *              a. add pv size printf
 *	pv14, 20120327 xu.gao
 *		a. add interface for pvsize switch between 8/8 and 6/8
 *	pv15, 20120413 dongdong.wang
 *		a. add speedy capture
 *	pv16, 20120502 linjie.cheng
 *		a. add get and set zoom ctrl parameter api for host use. (video and still)
 *	pv17, 20120601 linjie.cheng
 *		a. fixed non-blocking intr zoom auto set max factor, min factor, zoom step, HQdzoom step
 *	pv18, 20120619 linjie.cheng
 *		a.fixed smooth zoom flow
 *	pv19, 20120716 linjie.cheng
 *		a.fixed one step zoom ae fail
 *	pv20, 20120724 rf.wang
 *		a.fixed 3 buf mode video bad fram bug
 *	pv21, 20120727 rf.wang
 *		a.add blcoking zoom and nonblocking zoom
 *	pv22, 20130318 linjie.cheng
 *           a. add panorama function 
 *           b. adjust motion detect flow
 *************************************************************************/
 
/**
 * @file      pv_libver.c
 * @brief     preview middleware library version
 * @author    Will Chiu
 * @since     2010-01-01
 * @date      2010-01-01
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
static UINT8 code verStr[] = "$pv22"; 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**
 * @fn        UINT8 pvLibVerGet(void)
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
libPvVerGet(
	void
)
{
	return (verStr + 3);
}

