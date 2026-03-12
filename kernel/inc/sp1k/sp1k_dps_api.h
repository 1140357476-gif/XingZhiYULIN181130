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

#ifndef __SP1K_DPS_API__
#define __SP1K_DPS_API__

/** \file sp1k_dps_api.h
 * @brief		Spca1628 DPS related interface.
 */

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "sp1k_def.h" 
#include "sidc_api.h"
#include "dps_api.h"
#include "sp1k_sidc_api.h"
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
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**
 * @fn        UINT8 sp1kDpsAction(UINT8 action, UINT32 param)
 * @brief     dps do action
 * @param     [in] action	dps action idx
 * @param     [in] param	dps action param
 * @retval    return = DPS_ACTION_Idle / DPS_ACTION_Err
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 sp1kDpsAction(UINT8 action, UINT32 param);

/**
 * @fn        UINT8 Sp1kDpsAddImage(UINT32 handle, UINT8 *pfileName, UINT16 copies, UINT16 cropping_x, UINT16 cropping_y, UINT16 cropping_w, UINT16 cropping_h, UINT8 *pDate)
 * @brief     dps add image to print list
 * @param     [in] handle        the ID of file
 * @param     [in] pfileName   filename
 * @param     [in] copies        print file count 
 * @param     [in] cropping_x  print area
 * @param     [in] cropping_y   
 * @param     [in] cropping_w  	   width
 * @param     [in] cropping_h   	   heigth
 * @param     [in] pDate         print date
 * @retval    return = print file list count
 * @author    wangjg
 * @since     2010-5-12
*/
UINT16 sp1kDpsAddImage(UINT32 handle, UINT8 *pfileName, UINT16 copies,
	UINT16 cropping_x, UINT16 cropping_y, UINT16 cropping_w, UINT16 cropping_h,
	UINT8 *pDate
	);

UINT8 sp1kDpsRemoveImage(UINT8 idx);
/**
 * @fn        UINT8 sp1kDpsClearImageList(void)
 * @brief     clear printf list
 * @param     NONE
 * @retval    return = TRUE / FALSE
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 sp1kDpsClearImageList();
/**
 * @fn        UINT16 sp1kDpsGetServiceInfo(tService pcps)
 * @brief     get printer service info
 * @param     [out] pcps   the descriptor data pointer of printer service
 * @retval    return = is info new or not
 * @author    wangjg
 * @since     2010-5-12
*/
UINT16 sp1kDpsGetServiceInfo(PDPS_PARA_ConfigPrintService pcps);

/**
 * @fn        UINT16 sp1kDpsGetCapability(UINT8 type, PDPS_PARA_Capability_Elem pce)
 * @brief     get capability data
 * @param     [in] type   the descriptor data pointer of capability
 * @param     [out] pce   capability data
 * @retval    return = is info is new or not:0 is old, 
 * @author    wangjg
 * @since     2010-5-12
*/
UINT16 sp1kDpsGetCapability(UINT8 type, PDPS_PARA_Capability_Elem pce);
/**
 * @fn        UINT16 sp1kDpsGetDeviceStatus(PDPS_PARA_DeviceStatus pds)
 * @brief     ger device status data
 * @param     [out] pds     the descriptor data pointer of  device status 
 * @retval    return = is data new or not
 * @author    wangjg
 * @since     2010-5-12
*/
UINT16 sp1kDpsGetDeviceStatus(PDPS_PARA_DeviceStatus pds);
/**
 * @fn        UINT16 sp1kDpsGetJobStatus(PDPS_PARA_JobStatus pjs)
 * @brief     get job status data
 * @param     [out] pjs    the descriptor data pointer of job status
 * @retval    return = is data new or not
 * @author    wangjg
 * @since     2010-5-12
*/
UINT16 sp1kDpsGetJobStatus(PDPS_PARA_JobStatus pjs);
/**
 * @fn        UINT8 sp1kDpsGetPrintCapability(UINT8 type, UINT32 *pValue)
 * @brief     get print capability data
 * @param     [in] type         print capability type
 * @param     [out] pValue    print capability value
 * @retval    return =  TRUE / FALSE
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 sp1kDpsGetPrintCapability(UINT8 type, UINT32 *pValue);
/**
 * @fn        UINT8 sp1kDpsSetPrintCapability(UINT8 type, type,UINT32 value)
 * @brief     set print capability
 * @param     [in] type      print capability type
 * @param     [in] value     print capability value
 * @retval    return = TRUE / FALSE
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 sp1kDpsSetPrintCapability(UINT8 type,UINT32 value);
/**
 * @fn        UINT8 sp1kDpsGetPrintConfigInfo(UINT8 index, PDPS_PARA_ConfigPrintInfo pcpi)
 * @brief     get config info in print list 
 * @param     [in] index    the index of print file list
 * @param     [out] pcpi    the data pointer
 * @retval    return = TRUE / FALSE
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 sp1kDpsGetPrintConfigInfo(UINT8 index, PDPS_PARA_ConfigPrintInfo pcpi);

/**
 * @fn        UINT32 sp1kDpsSetParam(UINT8 idx,UINT32 val)
 * @brief     set dps param
 * @param     [in] idx
 * @param     [in] val :0xffffffff return current param
 * @retval    return = current param value
 * @author    wangjg
 * @since     2010-5-12
*/
UINT32 sp1kDpsSetParam(UINT8 idx, UINT32 val);


/**
 * @fn        UINT8 sp1kDpsGetException()
 * @brief     get exception status
 * @param     none
 * @param     none
 * @retval    return = current exception status
 * @author    wangjg
 * @since     2011-3-12
*/

UINT8 sp1kDpsGetException();


/**************************************************************************
 *               old api               *
 **************************************************************************/

/**
 * @fn UINT8 sp1k_dps_setPrtCapability(UINT8 type,UINT32 value) 
 * @brief						set the print param of this list of image files
 							if the printer not support the capability,it will set default value.
 * @param[in] type 			the type of print param 
 * @param[in] value  	      		the value of print param	
 * @retval TRUE/FALSE
 * @see dps_api.h dps_api.c
 */
 
#define sp1k_dps_setPrtCapability(UINT8_type,UINT32_value)				sp1kDpsSetPrintCapability(UINT8_type,UINT32_value)

/**
 * @fn        UINT32 sp1kDpsSetParam(UINT8 idx,UINT32 val)
 * @brief     set dps param
 * @param     [in] idx
 * @param     [in] val :0xffffffff return current param
 * @retval    return = current param value
 * @author    wangjg
 * @since     2010-5-12
*/

#define sp1k_dps_setParam(idx, val) sp1kDpsSetParam(idx,val)

/**
 * @fn UINT8 sp1k_dps_getPrtCapability(UINT8 type,UINT32 value) 
 * @brief						get the print param of this list of image files
 * @param[in] type 			the type of print param 
 * @param[in] value  	      		the value of print param	
 * @retval TRUE/FALSE
 * @see dps_api.h dps_api.c
 */
#define sp1k_dps_getPrtCapability(UINT8_type,UINT32_value)					sp1kDpsGetPrintCapability(UINT8_type,UINT32_value)

/**
 * @fn UINT8 Sp1kDpsAddImage(UINT32 handle,UINT8 *pfileName,UINT16 copies,
	UINT16 cropping_x,UINT16 cropping_y,UINT16 cropping_w,UINT16 cropping_h,
	UINT8 *pDate	)
 * @brief						add a image file to print list
 * @param[in] handle 			the ID of file
 * @param[in] pfileName 		print filename
 * @param[in] copies			print count
 * @param[in] cropping_x 		print area
 * @param[in] cropping_y 
 * @param[in] cropping_w 
 * @param[in] cropping_h 
 * @param[in] pDate 			print date
 * @retval print file list count
 * @see dps_api.h dps_api.c
 */
#define sp1k_dps_addImageToPrint(handle,pfileName,copies, cropping_x, cropping_y, cropping_w, cropping_h,pDate) sp1kDpsAddImage(handle,pfileName,copies, cropping_x, cropping_y, cropping_w, cropping_h,pDate)
/**
 * @fn UINT8 sp1k_dps_doAction(UINT8 action,UINT32 param) 
 * @brief						start a dps action
 * @param[in] action 			the type of action
 * @param[in] param  	      		 	
 * @retval DPS_ACTION_Idle
 * @see dps_api.h dps_api.c
 */
#define sp1k_dps_doAction(UINT8_action,UINT32_param) 					sp1kDpsAction(UINT8_action,UINT32_param)


/**
 * @fn UINT16 sp1k_dps_getPrtServiceInfo(PDPS_PARA_ConfigPrintService pcps)
 * @brief						get the descriptor data of printer service
 * @param[in] pcps 			the descriptor data pointer of printer service
 * @retval 					the data is new or not
 * @see dps_api.h dps_api.c
 */
#define sp1k_dps_getPrtServiceInfo(pDPS_PARA_ConfigPrintService) 		sp1kDpsGetServiceInfo(pDPS_PARA_ConfigPrintService)

/**
 * @fn UINT16 sp1kDpsGetCapability(UINT8 type,PDPS_PARA_Capability_Elem pce)
 * @brief						get the descriptor data of printer capability
 * @param[in] pcps 			the descriptor data pointer of printer capability
 * @retval 					the data is new or not
 * @see dps_api.h dps_api.c
 */
#define sp1k_dps_getCapability(UINT8_type,pDPS_PARA_Capability_Elem) 	sp1kDpsGetCapability(UINT8_type,pDPS_PARA_Capability_Elem)


/**
 * @fn UINT16 dps_getStatus(PDPS_PARA_JobStatus pjs,PDPS_PARA_DeviceStatus pds)
 * @brief						get the descriptor data of printer job/device status
 * @param[in] pcps 			the descriptor data of printer job/device status
 * @retval 					the data is new or not
 * @see dps_api.h dps_api.c
 */
#define sp1k_dps_getStatus(pDPS_PARA_JobStatus ,pDPS_PARA_DeviceStatus )	if(pDPS_PARA_JobStatus)sp1kDpsGetJobStatus(pDPS_PARA_JobStatus );\
																			else if(pDPS_PARA_DeviceStatus)sp1kDpsGetDeviceStatus(pDPS_PARA_DeviceStatus );
/**
 * @fn        UINT16 sp1kDpsGetJobStatus(PDPS_PARA_JobStatus pjs)
 * @brief     get job status data
 * @param     [out] pjs    the descriptor data pointer of job status
 * @retval    return = is data new or not
 * @author    wangjg
 * @since     2010-5-12
*/																			
#define sp1k_dps_getJobStatus(pDPS_PARA_JobStatus) sp1kDpsGetJobStatus(pDPS_PARA_JobStatus )

/**
 * @fn        UINT16 sp1kDpsGetDeviceStatus(PDPS_PARA_DeviceStatus pds)
 * @brief     ger device status data
 * @param     [out] pds  the descriptor data pointer of device status
 * @retval    return = is data new or not
 * @author    wangjg
 * @since     2010-5-12
*/

#define sp1k_dps_getDeviceStatus(pDPS_PARA_DeviceStatus) sp1kDpsGetDeviceStatus(pDPS_PARA_DeviceStatus )

/**
 * @fn UINT8 sp1kDpsGetPrintConfigInfo(UINT8 index,DPS_PARA_ConfigPrintInfo pcpi)
 * @brief						get the data of the print file list
 * @param[in] pcpi 			the index of the print file list
 * @param[out] pcpi 			the data pointer 
 * @retval 					the data is new or not
 * @see dps_api.h dps_api.c
 */
#define sp1k_dps_getPrtConfigInfo( index,pDPS_PARA_ConfigPrintInfo)		sp1kDpsGetPrintConfigInfo( index,pDPS_PARA_ConfigPrintInfo)

/**
 * @fn        void sp1k_dps_makeHandle( fileIndex)
 * @brief     make sidc handle from index
 * @param     [in] fileIndex
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
#define  sp1k_dps_makeHandle( fileIndex)  sp1kSidcMakeHandle( fileIndex) 

/**
 * @fn        void sp1k_dps_sendEvent( ec, ec, p0, ec,p0, p1, ec,p0,p1, p2)
 * @brief     send sidc event to printf
 * @param     [in] ec   eventcode
 * @param     [in] p0
 * @param     [in] p1
 * @param     [in] p2
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
#define sp1k_dps_sendEvent(ec,p0,p1,p2) sp1kSidcSendEvent(ec,p0,p1,p2)

/**
 * @fn        void sp1k_dps_sidcInit(void)
 * @brief     function is invalid now
 * @param     NONE
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
#define sp1k_dps_sidcInit() 
/**
 * @fn        UINT8 sp1kDpsGetException()
 * @brief     get exception status
 * @param     none
 * @param     none
 * @retval    return = current exception status
 * @author    wangjg
 * @since     2011-3-12
*/

#define sp1k_dps_getException sp1kDpsGetException()
#endif

