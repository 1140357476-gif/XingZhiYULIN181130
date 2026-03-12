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
 * @file      os_mutex.c
 * @brief     Mutex operation for resource control
 * @author    Phil Lin
 * @since     2011-04-01
 * @date      2011-04-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h" 
#include "os_mutex.h"
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
static UINT32 xdata osMutex = -1;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**
 * @fn        void osMutexInit(void)
 * @brief     initialize the mutex buffer
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-4-1
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
void
osMutexInit(
	void
)
{
	osMutex = -1;
}

/**
 * @fn        UINT8 osMutexQuery(osMutexIdx_t mutex)
 * @brief     Query the specified mutex
 * @param     [in] mutex
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-4-1
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
UINT8
osMutexQuery(
	osMutexIdx_t mutex
)
{
	if (mutex >= OS_MUTEX_MAX) {
		return FAIL;
	}
	
	if (osMutex & (0x01 << mutex)) {		
		return SUCCESS;
	}

	return FAIL;
}
/**
 * @fn        UINT8 osMutexGet(osMutexIdx_t mutex)
 * @brief     Get the specified mutex
 * @param     [in] mutex
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-4-1
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
UINT8
osMutexGet(
	osMutexIdx_t mutex
)
{
	if (osMutexQuery(mutex) == SUCCESS) {
		osMutex &= ~(0x01 << mutex);
		return SUCCESS;
	}
	
	return FAIL;
}

/**
 * @fn        void osMutexFree(osMutexIdx_t mutex)
 * @brief     Free the specified mutex
 * @param     [in] mutex
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-4-1
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
void
osMutexFree(
	osMutexIdx_t mutex
)
{
	if (mutex < OS_MUTEX_MAX) {
		osMutex |= (0x01 << mutex);
	}
}
