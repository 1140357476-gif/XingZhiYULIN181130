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

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#define __FILE   __FILE_ID_SYS_SVC_DEF__
#include "general.h" 
#define SYS_SRC_INTERNAL //for libSys internal use only
#include "sys_svc_def.h" 
#include "cmd.h" 
#include "dbg_def.h" 
#include "sp1k_front_api.h"
#include "utility.h"

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
extern void usbSvcProc(void * ptr);
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*
  Note: The array size can't be lager than FUNC_SYS_SVC_MAX 
*/
SYS_SVC_DCL const sysSvc_t code sysSvcArray[] = {
	{"cmd",	cmdMonitor},
	{"usb",	usbSvcProc},
	{"vd", vdSvcProc},
	{"cs", csSvcProc}, /* For code sentry use */
	{NULL, NULL},
};

/**
 * @brief     sysSvcSched
 * @param     [in] ptr
 * @retval    NONE
 * @see       none
 * @author    Phil Lin
 * @since     2010-01-01
 * @todo      N/A
 * @bug       N/A
*/
void 
sysSvcSched(
	void* ptr
)
{
	sysSvc_t code* pSvc;
	UINT8 svcCnt;

	/* system service switch to idle */
	pSvc = &sysSvcArray[0];
	svcCnt = 0;

	/* system service */
	while(pSvc->svcProc) {
		pSvc->svcProc(ptr);
		pSvc++;
		if (++svcCnt >= FUNC_SYS_SVC_MAX) {
			/* sysSvcArray configure error */ 
			ASSERT(0, 0);
		}
	}
}
