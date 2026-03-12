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

#ifndef __SP1K_SIDC_API_H__
#define __SP1K_SIDC_API_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "sidc_param.h" 

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
 * @fn        void sp1kSidcSendEvent(UINT32 eventCode, UINT32 param0, UINT32 param1, UINT32 param2)
 * @brief     send sidc event 
 * @param     [in] eventCode
 * @param     [in] param0
 * @param     [in] param1
 * @param     [in] param2
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
void sp1kSidcSendEvent(
	UINT32 eventCode,
	UINT32 param0,
	UINT32 param1,
	UINT32 param2
) ;
/**
 * @fn        UINT32 sp1kSidcMakeHandle(UINT16 fileIndex)
 * @brief     create file handle in current folder
 * @param     [in] fileIndex
 * @retval    return = handle
 * @author    wangjg
 * @since     2010-5-12
*/
UINT32 sp1kSidcMakeHandle(UINT16 fileIndex);
/**
 * @fn        UINT8 sp1kSidcParsingHandle(UINT32 handle, UINT8 *fileName)
 * @brief     parse handle to full path
 * @param     [in] handle
 * @param     [out] fileName
 * @retval    return = TRUE / FALSE
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 sp1kSidcParsingHandle(
	UINT32 handle, 
	UINT8 *fileName);

/**
 * @fn        void sp1kSidcInit(void)
 * @brief     Init Sidc 
 * @param     void
 * @retval    null
 * @author    wangjg
 * @since     2010-5-12
*/
void sp1kSidcInit();

#endif  /* __SP1K_SIDC_API_H__ */

