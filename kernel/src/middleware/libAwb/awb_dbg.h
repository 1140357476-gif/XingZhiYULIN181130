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
 * @file      awb_dbg.h
 * @brief     awb lib all function
 * @author   wangjg
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __AWB_DBG_H__
#define __AWB_DBG_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define AWB_Debug_0	0
#define AWB_Debug_1	0
#define AWB_Debug_Print 0
#define AWB_Debug_PrintLoop 0
#define AWB_Debug_PrintInfo 0
#define AWB_Debug_Cmd 0

#define AWB_Debug_PrintTimer 0



//#define awbPrintfTimer printf
//#define awbPrintfDecider printf
//#define awbPrintfFilter printf
//#define awbPrintfSpotDecider printf


#define _awbPintf_ /

#ifndef awbPrintfTimer
#define awbPrintfTimer _awbPintf_/printf
#define awbGetTimer(t)
#else
#define awbGetTimer(t) HAL_GlobalTimerRead10us(t)
#endif
#ifndef awbPrintfDecider 
#define awbPrintfDecider _awbPintf_/printf
#endif
#ifndef awbPrintfFilter 
#define awbPrintfFilter _awbPintf_/printf
#endif
#ifndef awbPrintfSpotDecider 
#define awbPrintfSpotDecider _awbPintf_/printf
#endif

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/


#endif  /* __TEMPLATE_HDR_H__ */

