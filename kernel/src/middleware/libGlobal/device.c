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
 * @file      device.c
 * @brief     for device application
 * @author    Phil Lin
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "gpio.h"
#include "smcfunc.h"
#include "dos32.h"
#include "sp1k_gpio_api.h" 
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
 static devAttr_t xdata sSysDev[DEV_ID_MAX];
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

//-----------------------------------------------------------------------------
// sp1kDevInit
//-----------------------------------------------------------------------------
/**
 * @fn        UINT8 sp1kDevInit(void)
 * @brief     sp1kDevInit
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-21
 * @todo      N/A
 * @bug       N/A
*/
UINT8 sp1kDevInit(void)
{
	devAttr_t* pDev = sSysDev;
	UINT8 DevNum = DEV_ID_MAX;
	UINT8 i;

	for (i = 0; i < DevNum; i++) {
		pDev[i].devId = i;
		pDev[i].devEn = 0;
		pDev[i].devSts = DEV_STS_DISCONNECTED;
		pDev[i].devType = DEV_TYPE_UNKNOWN;
		//strcpy(pDev[i].devDrv, "0001");
	}

	/* Disk attrib initialization */
	if (DOS_DosRamStsGet() == TRUE) {
		pDev[DEV_ID_DISK].devType = DEV_TYPE_DISK_DRAM;
	} else {
		pDev[DEV_ID_DISK].devType = sp1kDiskTypeGet();
	}

	/* Panel / TV attrib initialization */
	if(!sp1kTV_PlugGet()) {

		pDev[DEV_ID_PANEL].devSts = DEV_STS_CONNECTED;
		pDev[DEV_ID_PANEL].devEn = 0;
		pDev[DEV_ID_TV].devSts = DEV_STS_CONNECTED;
		pDev[DEV_ID_TV].devEn = 1;
		pDev[DEV_ID_TV].devType = DEV_TYPE_TV_COM;
		printf("init TV\n");
	} else {
		pDev[DEV_ID_PANEL].devSts = DEV_STS_CONNECTED;
		pDev[DEV_ID_PANEL].devEn = 1;
		pDev[DEV_ID_PANEL].devType = DEV_TYPE_PANEL_COM;
		pDev[DEV_ID_TV].devSts = DEV_STS_DISCONNECTED;
		pDev[DEV_ID_TV].devEn = 0;
	}

	return SUCCESS;
}

//-----------------------------------------------------------------------------
// sp1kDevHdlGet
//-----------------------------------------------------------------------------
/**
 * @fn        devAttr_t sp1kDevHdlGet(void)
 * @brief     sp1kDevHdlGet
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-21
 * @todo      N/A
 * @bug       N/A
*/
devAttr_t* sp1kDevHdlGet(void)
{
	return &sSysDev[0];
}

//-----------------------------------------------------------------------------
// sp1kDevHdlGet
//-----------------------------------------------------------------------------
/**
 * @fn        UINT8 sp1kDevInfoUpdate(devId_t devId, st pDevAttr)
 * @brief     sp1kDevInfoUpdate
 * @param     [in] devId
 * @param     [in] pDevAttr
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-21
 * @todo      N/A
 * @bug       N/A
*/
UINT8 sp1kDevInfoUpdate(devId_t devId, devAttr_t* const pDevAttr)
{
	devAttr_t* pDev = sSysDev;
	UINT8 DevNum = DEV_ID_MAX;

	if (devId > DevNum || pDev == NULL) {
		return FAIL;
	}	

	//pDevAttr[devId].devId = pDev->devId;
	pDevAttr[devId].devEn = pDev->devEn;
	pDevAttr[devId].devSts = pDev->devSts;
	//pDevAttr[devId].devDrv = pDev->devDrv;

	return SUCCESS;
}


