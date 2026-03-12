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
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __HAL_NAND_H__
#define __HAL_NAND_H__

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
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT8 HAL_NANDInit(UINT8 , UINT8);
UINT8 HAL_NANDSendCmd(UINT8);
UINT8 HAL_NANDSendAddr(UINT8,UINT32);
void  HAL_NANDCheckRdy(UINT8*);
void  HAL_NANDWritePort(UINT8);
UINT8 HAL_NANDReadPort(void);
void  HAL_NANDCompleteOperation(void);
void HAL_NANDEnableWriteProtect(UINT8 en);
/*Phil add new MTD driver s*/
UINT8 HAL_NANDChkRdy(void);
UINT8 HAL_NANDAutoCmdCfg(UINT8 xdata* pCmd, UINT8 CmdCnt, UINT8 xdata* pAddr, UINT8 AddrCnt);
void HAL_NANDSetDRAMDMA(UINT32 DRAMAddrWord);
void HAL_NANDDoDMACfg(UINT8 Src,UINT8 Dst, UINT32 nByte, UINT8 FlashPageDMA);
UINT8 HAL_NANDDoDMAWait(void);
void HAL_NANDSramFlagSet(void);

#endif  /* __HAL_NAND_H__ */
