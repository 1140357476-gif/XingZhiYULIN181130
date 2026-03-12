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
 * @file      pv_task.h
 * @brief     preview header file
 * @author    Will Chiu
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __PV_TASK_H__
#define __PV_TASK_H__


/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "app_video_view.h"
#include "initio.h"
#include "pv_zoom.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#if  SUPPORT_QVGA_SENSOR
#define StillCDSPMaxFac				(UINT16)100		/* LinJie: chang it from 140 to 200 */
#define VideoCDSPMaxFac				(UINT16)100		/* will: change it from 140 to 200 */
#define VideoCDSPMaxFac_VGA_60FPS	(UINT16)100		/* LinJie: vga 60fps bandwidth problem, so all display zoom */
#else
#define StillCDSPMaxFac				(UINT16)100		/* LinJie: chang it from 140 to 200 */
#define VideoCDSPMaxFac				(UINT16)100		/* will: change it from 140 to 200 */
#define VideoCDSPMaxFac_VGA_60FPS	(UINT16)100		/* LinJie: vga 60fps bandwidth problem, so all display zoom */
#endif

#define VGA_60FPS_2ST_PATH_EN			1

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

void pvSizeSet(UINT16 hSize, UINT16 vSize) ;
void pvSizeGet(UINT16 *hSize, UINT16 *vSize) ;
void pvIntrSizeGet(UINT16 *hSize, UINT16 *vSize) ;


void pvClampSizeGet(UINT16 *hSize, UINT16 *vSize);
UINT32 pvFreeBuffGet(void);
//void pvBufSave(void);
void pvFwZFSet(UINT8 val);

#endif  /* __DISP_API_H__ */

