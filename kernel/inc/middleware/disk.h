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
 * @file      disk.h
 * @brief     Brief
 * @author    Xianxin
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __DISK_H__
#define __DISK_H__

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
typedef enum {
	DISK_ERR_NONE		= (UINT8)0,
	DISK_ERR_DETECT 	= (UINT8)1,
	DISK_ERR_FS_INIT	= (UINT8)2, 
}DISK_ERR_CODE_e;
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT8 diskInitialize(UINT8 doDOSInitial);
UINT8 diskDetect(void);
UINT8 diskFormat(UINT8 eraseCard);
void diskMemInit(void);
void diskGlobalInit(void);
void diskInactivate(void);

UINT8 diskPhySectorRead(UINT32 logAddr, UINT32 phyAddr, UINT32 ramAddr);
UINT8 diskSectorRead(UINT32 logAddr, UINT32 ramAddr);
UINT8 diskPhyBlockWrite(UINT32 logAddr, UINT32 phyAddr, UINT16 blockNum, UINT32 ramAddr);
UINT8 diskSectorWrite(UINT32 logAddr, UINT32 ramAddr);

UINT8 diskErrorCodeGet(void);

#endif  /* __DISK_H__ */
