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

#ifndef __SD_PRIVATE_H__
#define __SD_PRIVATE_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define SD_MODE_POST_IDLE_SET(val)         do{sdModePostIdle=(val);}while(0)
#define SD_MODE_POST_IDLE_GET()            sdModePostIdle
//#define MMC_SUPPORT_SET(val)                     do{mmcSupport=(val);}while(0)
//#define MMC_SUPPORT_GET()                          mmcSupport
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT8 RspBuf[];
extern UINT8 sdPageShift;
extern UINT8 sdByteMode;
extern UINT32 G_SD_MultiSecNum;
extern UINT8 G_SD_UnderWrite;
extern UINT8 sdModePostIdle;

//temp from other modulde
extern xdata UINT8 G_USBReset;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT8 L1_SDIdentification(UINT8, UINT8, UINT16);
UINT8 L1_DMAWrSD(UINT8, UINT32 , UINT32, UINT16 );
UINT8 L1_DMARdSD(UINT8, UINT32 , UINT32, UINT16 );
UINT8 L1_DMARdSDBulk2(UINT8 Dst, UINT32 SAdr, UINT32 BlockCnt, UINT16 BlockSize);

UINT8 L1_SDCheckState(void);
UINT8 L1_SDSetBusMode(UINT8, UINT8, UINT8);
UINT8 L1_SDEraseData(UINT32 , UINT32 );

UINT8 L1_SDUsb2CardDmaWait(UINT32 wrt_num);
UINT8 L1_SDCard2UsbDmaWait(UINT32 rd_num);

#endif  /* __SD_PRIVATE_H__ */
