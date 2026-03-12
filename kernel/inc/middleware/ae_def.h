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
 * @file      File name
 * @brief     Brief
 * @author    wangjg
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __AE_DEF_H__
#define __AE_DEF_H__

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
#define AE_MOVE_DETECT_EN	0
#define AE_NO_USED			0

#define AE_SHOW_INFO		1	/* Show AE Current gId in Panel */

#define II_AE_MOTION_DET_EN	0	/* YUV sensor, motion detected use II AE :1, Motion detected not use II AE :0 */


#if 1
void aeMoeTrig(void);
void aeIntrProc(void);

//void aeSetMetering();

#define aeMoeTrigApi() aeMoeTrig()
#define aeIntrProcApi() aeIntrProc()
//#define aeSetMeteringApi() aeSetMetering()
#endif

#if 1//AE_II
void aeIITrig();
void aeInit(UINT8 proc);

#define aeIITrigApi() aeIITrig()
#endif 

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT8 aeGainExpSetEn ;
extern UINT16 aeWndAbsErr;
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/**
 * @fn        void aeAwbInfo(UINT8* ptGid, ptGid,UINT8* ptLuma, ptLuma,UINT8* ptContrast)
 * @brief     set para to awb
 * @param     [in] ptGid
 * @param     [in] ptLuma
 * @param     [in] ptContrast
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-7-16
 * @todo      N/A
 * @bug       N/A
*/
//void aeAwbInfo(UINT8* ptGid,UINT8* ptLuma,UINT8* ptContrast);
void aeAwbInfo(UINT8* ptGid,UINT8* ptLuma,UINT8* ptymax, UINT8* ptymin);

#endif  /* __TEMPLATE_HDR_H__ */
