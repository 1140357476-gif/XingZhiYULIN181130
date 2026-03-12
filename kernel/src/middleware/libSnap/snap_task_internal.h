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
 * @author    Author's name
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef _SNAP_TASK_INTERNAL_H_
#define _SNAP_TASK_INTERNAL_H_

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
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
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
	extern	xdata UINT32 snapRawDataAddr;
	extern	xdata UINT32 snapYuv1Addr;
	extern	xdata UINT32 snapYuv2Addr;
	
	extern xdata UINT32 snapCdspYuvToYuvBufAddr0;
	extern xdata UINT32 snapCdspYuvToYuvBufAddr1;
	
	extern xdata UINT16 snapRawHSize;
	extern xdata UINT16 snapRawVSize;
	
	extern xdata UINT16 snapYuvHSize;
	extern xdata UINT16 snapYuvVSize;

	extern xdata UINT16 snapQvHSize;
	extern xdata UINT16 snapQvVSize;	
	
	extern xdata UINT16 snapYuvLines;
	
	extern xdata UINT8 snapYuvABBufOpt;
	
	extern xdata UINT8 snapBurstOpt;
	
	extern xdata UINT8 snapRawMode;
	
	extern xdata UINT16 snapSensorCropHSize;
	extern xdata UINT16 snapSensorCropVSize;
	
	extern xdata UINT32 snapJpegVlcAddr;

	extern xdata UINT16 snapHSize;
	extern xdata UINT16 snapVSize;
	extern xdata UINT16 snapHFactor;
	extern xdata UINT16 snapVFactor;

	extern xdata UINT32 snapQvAddr; 
	
	extern xdata UINT32 snapQvVlcAddr; 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/



#endif  /* _TEMPLATE_HDR_H_ */
