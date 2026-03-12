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

#ifndef _HAL_SPI_H_
#define _HAL_SPI_H_

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
typedef enum {
	SP1K_SPI_CALLBACK_WP_SET = 0,
	SP1K_SPI_CALLBACK_WP_MAX
} SfCallback_e;

typedef enum{
	SPI_STS_UNLOCK		= (UINT8)0x00,
	SPI_STS_LOCK_0		= (UINT8)0x01,
	SPI_STS_LOCK_ALL	= (UINT8)0x02,
	SPI_STS_LOCK_MAX	= (UINT8)0xFF,
}spiLockSts_e;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void HAL_SPIWritePort(UINT8 PortData);
UINT8 HAL_SPIReadPort(void);
UINT8 HAL_SPIInit(UINT8 Freq,UINT8 Mode);
void sfCallbackReg(UINT32 idx,void * pfunct);

#endif  /* _HAL_SPI_H_ */
