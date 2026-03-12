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

#ifndef __SP1K_UVC_API_H__
#define __SP1K_UVC_API_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "common.h"
#include "Ctlsetup.h" 
#include "class.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct
{
	CTRL_COMMAND cmd;
	UINT8* pData;
} usbVNDCb_t, *pusbVNDCb_t;

typedef UINT16 (code *usbcbVNDRFunc_t)(pusbVNDCb_t pCb);

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
 * @fn        void sp1kUvcUserExtUnitPreInCallBackSet(usbcbVNDRFunc_t pfun)
 * @brief     set callback function for UVC extension unit command defined by user
 * @pfun     [in] funciont pointer
 * @retval    NONE
 * @author    xianxin
 * @since     2014-7-2
*/
void sp1kUvcUserExtUnitPreInCallBackSet(usbcbVNDRFunc_t pfun);

/**
 * @fn        void sp1kUvcUserExtUnitPostOutCallBackSet(usbcbVNDRFunc_t pfun)
 * @brief     set callback function for UVC extension unit command defined by user
 * @pfun     [in] funciont pointer
 * @retval    NONE
 * @author    xianxin
 * @since     2014-7-2
*/
void sp1kUvcUserExtUnitPostOutCallBackSet(usbcbVNDRFunc_t pfun);

#endif  /* __SP1K_UVC_API_H__ */

