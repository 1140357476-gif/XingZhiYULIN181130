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
 * @author    XianXin
 * @since     2010-04-28
 * @date      2010-04-28
*/

#ifndef __HAL_SD_H__
#define __HAL_SD_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define	SD_TIME_OUT	(XBYTE[REG_Fm_SD_Status] & (1<<6))
#define	SD_CRC16_OK	!(XBYTE[REG_Fm_evt] & 0x08)//(XBYTE[REG_Fm_CRC16cor])
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef enum {
	HAL_SD_PHASE_NORMAL = 0,
	HAL_SD_PHASE_HI_READ,
	HAL_SD_PHASE_HI_WRITE,
} HAL_SDPhaseSel_e;
  
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void HAL_SDInit(UINT8 mmcMode);
void HAL_SDConfig(UINT8 OpFreq, UINT8 DataBusWidth, UINT8 mmc);
void HAL_SDBlockSize(UINT16 BlockSize);
UINT8 HAL_SDTxCommand(UINT8 cmd, UINT32 arg, UINT8 RspType, UINT8 *Buff);
UINT8 HAL_SDTxDummy(void);
UINT8 HAL_SDWaitIdle(void);
UINT8 HAL_SDCmdWaitIdle(void);
UINT8 HAL_SDDram(UINT32 dramAddr, UINT32 nByte1, UINT8 read);
void HAL_SDHighSpeedStsSet(UINT8 en);
UINT8 HAL_SDHighSpeedStsGet(void);
void HAL_SDPhaseChange(UINT8 sel);

UINT8 INTR_HAL_SDDram(UINT32 dramAddr, UINT16 nByte1, UINT8 read);

#endif  /* __HAL_SD_H__ */

