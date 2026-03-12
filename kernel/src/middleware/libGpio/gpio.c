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
 * @file      gpio.c
 * @brief     get chip package pin
 * @author    kai.wang
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "gpio.h"
#include "gpio_api.h"
#include "app_dev_storage.h"
#include "app_dev_usb.h"
#include "app_dev_disp.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define USE_PKG_DEDICATE_PIN  1  /* 0: old code, 1: new code. */
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
extern code pkgDedicateGpio_t pkgDedicateGpioTbl[];
extern code UINT8 pkgDedicateGpioTot;
extern code UINT16 pkgChipId; 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/**
 * @fn        UINT8 pkgDedicateGpioGet(UINT8 pinId, UINT8 *pbyteId, UINT8 *pbitMsk, UINT8 *ppolarity)
 * @brief     get dedicate gpio pin in current chip package.
 * @param     [in] pinId: pin id, one of pkgDedicateGpioPin_e.
 * @param     [in] pbyteId: byte id, one of gpioByte_e.
 * @param     [in] pbitMsk: bitmap mask.
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-17
 * @todo	return values *pbyteId and *pbitMsk are used to call following api:
 *
 *          void  gpioByteFuncSet(UINT8 byteId, UINT8 msk, UINT8 func);
 *          UINT8 gpioByteFuncGet(UINT8 byteId, UINT8 msk);
 *          void  gpioByteDirSet(UINT8 byteId, UINT8 msk, UINT8 dir);
 *          UINT8 gpioByteDirGet(UINT8 byteId, UINT8 msk);
 *          void  gpioByteOutSet(UINT8 byteId, UINT8 msk, UINT8 val);
 *          UINT8 gpioByteOutGet(UINT8 byteId, UINT8 msk);
 *          void  gpioByteInGateSet(UINT8 byteId, UINT8 msk, UINT8 gate);
 *          UINT8 gpioByteInGet(UINT8 byteId, UINT8 msk, UINT8 polarity);
 * @bug       N/A
*/
UINT8
pkgDedicateGpioGet(
	UINT8 pinId,
	UINT8 *pbyteId,
	UINT8 *pbitMsk,
	UINT8 *ppolarity
)
{
	UINT8 idx;


	/* search desired dedicate pin. */
	for ( idx = 0; idx < pkgDedicateGpioTot; idx++ ){
		if ( pinId == pkgDedicateGpioTbl[idx].pinId ) {
			break;
		}
	}

	if ( idx < pkgDedicateGpioTot ) {  /* found. */
		*pbyteId = pkgDedicateGpioTbl[idx].byteId;
		*pbitMsk = pkgDedicateGpioTbl[idx].bitMsk;
		*ppolarity = pkgDedicateGpioTbl[idx].polarity;
	}
	else {  /* not found. */
		*pbyteId = GPIO_BYTE_NULL;
		*pbitMsk = (UINT8)NULL;
	}

	if ( *pbyteId == GPIO_BYTE_NULL || *pbitMsk == NULL ) {
		*ppolarity = ACTIVE_HIGH;
		return FAIL;
	}
	else {
		return SUCCESS;
	}
}
#if USE_PKG_DEDICATE_PIN
/**
 * @fn        void pkgDedicateGpioCfg(UINT8 pinId, UINT8 *pbyteId, UINT8 *pbitMsk, UINT8*ppolarity, UINT8 dir)
 * @brief     configure package gpio 
 * @param     [in] pinId
 * @param     [in] pbyteId
 * @param     [in] pbitMsk
 * @param     [in] dir
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-17
 * @todo      N/A
 * @bug       N/A
*/
void
pkgDedicateGpioCfg(
	UINT8 pinId,
	UINT8 *pbyteId,
	UINT8 *pbitMsk,
	UINT8 *ppolarity,
	UINT8 dir
)
{
	UINT8 byteId, bitMsk, polarity;

	pkgDedicateGpioGet(pinId, &byteId, &bitMsk,&polarity);
	//gpioByteFuncSet(byteId, bitMsk, 0xFF);
	gpioByteDirSet(byteId, bitMsk, dir?0xFF:0x00);

	*pbyteId = byteId;
	*pbitMsk = bitMsk;
	*ppolarity = polarity;
}
#endif


//-----------------------------------------------------------------------------
//chipPackageIdGet
//-----------------------------------------------------------------------------
/*
UINT8 
pkgChipIdGet(
void
)
{
	return XBYTE[0x22E7];
}

*/

/**
 * @fn        UINT16 pkgChipIdGet(void)
 * @brief     get chip id in current chip package.
 * @param     NONE
 * @retval    chip id, 0x1525, 0x1526 or 0x1528.
 * @see       NULL
 * @author    Matt Wang
 * @since     2010-5-17
 * @todo      N/A
 * @bug       N/A
*/
UINT16
pkgChipIdGet(
	void
)
{
	return pkgChipId;
}


/**
 * @fn        UINT16 pkgChipIdGet_Intr(void)
 * @brief     get chip id in current chip package.
 * @param     NONE
 * @retval    chip id, 0x1525, 0x1526 or 0x1528.
 * @see       NULL
 * @author    Matt Wang
 * @since     2010-5-17
 * @todo      N/A
 * @bug       N/A
*/
UINT16
pkgChipIdGet_Intr(
	void
)
{
	return pkgChipId;
}

