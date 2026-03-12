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
 * @file      sp1k_disk_api.h
 * @brief     Brief	Spca1628 disk operation related interface
 * @author    XianXin
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __SP1K_DISK_API_H__
#define __SP1K_DISK_API_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "sp1k_def.h"
#include "disk.h"
 
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
	SD_SPEED_CLASS0 = (UINT8)0,
	SD_SPEED_CLASS2 = (UINT8)1,
	SD_SPEED_CLASS4 = (UINT8)2,
	SD_SPEED_CLASS6 = (UINT8)3,		
}SD_SPEED_CLASS_e;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
#ifdef SP1K_DOXY_GEN
/**
 * @fn UINT8 sp1kDiskFormat( UINT8 eraseCard )
 * @brief	To format the current storage drive
 * @param[in] eraseCard	    specify to do physical erase or not, especially nand drive
 *\n                              0: Not to do physical format.
 *\n                              1: Do physical format                               
 * @retval\n 
 *      TRUE  : format error.
 *\n    FALSE : format success                               
*/
UINT8 sp1kDiskFormat(UINT8 eraseCard);

/**
 * @fn UINT8 sp1kDiskInitialize( UINT8 doDOSInitial )
 * @brief	To do file system initialization
 * @param[in] doDOSInitial	To refresh file system buffer
 *\n      0: Not to do dos buffer refresh.
 *\n      1: Do dos buffer refresh, it's better to set doDOSInitial = 1.
 *\n         Because this will read all DOS related info ( such as FAT, directory ..)
 *\n         from storage to buffer.                      
 * @retval\n 
 *      TRUE  : initial error.
 *\n    FALSE : initial success                               
*/
UINT8 sp1kDiskInitialize( UINT8 doDOSInitial );

/**
 * @fn        void sp1kDiskMemInit(void)
 * @brief     Initialize memory for disk
 * @param     NONE
 * @retval    NONE
 * @see       NULL
*/
void sp1kDiskMemInit(void);

/**
 * @fn        void sp1kDiskGlobalInit(void)
 * @brief     Initialixe global for disk
 * @param     NONE
 * @retval    NONE
 * @see       NULL
*/
void sp1kDiskGlobalInit(void);

/**
 * @fn        UINT8 sp1kDiskErrorCodeGet(void)
 * @brief     Get the error code of disk operation
 * @param     NONE
 * @retval    return = SUCCESS / Error code, The error code is one of DISK_ERR_DETECT or DISK_ERR_FS_INIT.
 * @see       DISK_ERR_CODE_e.
*/
UINT8 sp1kDiskErrorCodeGet(void);

#else
#define sp1kDiskFormat  diskFormat
#define sp1kDiskInitialize  diskInitialize
#define sp1kDiskMemInit diskMemInit
#define sp1kDiskGlobalInit  diskGlobalInit
#define sp1kDiskErrorCodeGet diskErrorCodeGet
#endif

/**
 * @fn        UINT8 sp1kSDSpeedClassGet(void)
 * @brief     Get SD speed class
 * @param     NONE
 * @retval    return = Speed class, the value is SD_SPEED_CLASS0, SD_SPEED_CLASS2,SD_SPEED_CLASS4,SD_SPEED_CLASS6.
 * @see       SD_SPEED_CLASS_e
*/
UINT8 sp1kSDSpeedClassGet(void);

/**
 * @fn        UINT8 sp1kDosRamBufSet(UINT32 size)
 * @brief     Set size of DOS Ram.
 * @param     [in] size	DOS Ram size in word units.
 * @retval    return = TRUE
 * @see       NULL
*/
UINT8 sp1kDosRamBufSet(UINT32 size);

/**
 * @fn        void sp1kMmcSupportSet(UINT8 val)
 * @brief     Set if the MMC is supported.
 * @param     [in] val	The val is 1 if need support mmc, else set val to 0.
 * @retval    NONE
 * @see       NULL
*/
void sp1kMmcSupportSet(UINT8 val);
#endif  /* __SP1K_DISK_API_H__ */
