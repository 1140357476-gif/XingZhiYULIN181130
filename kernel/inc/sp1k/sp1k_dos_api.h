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
 * @file      sp1k_dos_api.h
 * @brief     Brief	Spca1628 DOS related interface
 * @author    XianXin
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __SP1K_DOS_API_H__
#define __SP1K_DOS_API_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "sp1k_def.h"
#include "dos32.h"
 
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
#ifdef SP1K_DOXY_GEN

/**
 * @fn        UINT8 sp1kDosRsvPartSet(UINT32 size)
 * @brief     Set the size for nand reserved partition
 * @param     [in] size	unit of BYTE.
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
*/
UINT8 sp1kDosRsvPartSet(UINT32 size); 

/**
 * @fn        UINT8 sp1kDosRsvPartStsGet(void)
 * @brief     Show whether the option for reserved partition function is enabled
 * @param     NONE
 * @retval    return		1: the reserved partition function is ENABLED\n
 *\n					0: the reserved partition function is DISABLED
 * @see       NULL
*/
UINT8 sp1kDosRsvPartStsGet(void);

/**
 * @fn        UINT32 sp1kDosRsvPartSizeGet(void)
 * @brief     Get the size of reserved partition
 * @param     NONE
 * @retval    return		the size of reserved partition (Unit of sectors).
 * @see       NULL
*/
UINT32 sp1kDosRsvPartSizeGet(void);

/**
 * @fn        sp1kDosRsvPartActSet(UINT8 en)
 * @brief     Active the reserved partition
 * @param     [in] en	1: Actived the reserved partition, thus, the all the DOS operation will be base on reserved partition\n
 *\n					0: Inactived the reserved partition
 * @retval    NONE
 * @see       NULL
*/
void sp1kDosRsvPartActSet(UINT8 en);

/**
 * @fn        void sp1kDosRsvPartWPSet(UINT8 wp)
 * @brief     Set the write protect attribute for reserved partition
 * @param     [in] wp	1: write protect is ENABLED for reserved partition\n
 *\n					0: write protect is DISABLED for reserved partition
 * @retval    NONE
 * @see       NULL
*/
void sp1kDosRsvPartWPSet(UINT8 wp);

/**
 * @fn        UINT8 sp1kDosRamStsGet(void)
 * @brief     Get DOS Ram status
 * @param     NONE
 * @retval    return		TRUE : enable, FALSE: disable.
 * @see       NULL
*/
UINT8 sp1kDosRamStsGet(void);

/**
 * @fn        UINT8 sp1kDosRamCfg(UINT8 enable)
 * @brief     Enable or disable DOS Ram.
 * @param     [in] enable	1: enable, 0:disable
 * @retval    return = SUCCESS
 * @see       NULL
*/
UINT8 sp1kDosRamCfg(UINT8 enable);

/**
 * @fn        UINT32 sp1kDosGetDiskSpace(void)
 * @brief     Get free space by KB
 * @param     NONE
 * @retval    return Free space of disk in KByte units.
 * @see       NULL
*/
UINT32 sp1kDosGetDiskSpace(void);

/**
 * @fn        UINT32 sp1kClusterSizeGet(void)
 * @brief     Get the cluster size.
 * @param     NONE
 * @retval    return cluster size in byte units.
 * @see       NULL
*/
UINT32 sp1kClusterSizeGet( void );

#else

#define sp1kDosRsvPartSet   DOS_RsvPartSet
#define sp1kDosRsvPartStsGet    DOS_RsvPartStsGet
#define sp1kDosRsvPartSizeGet   DOS_RsvPartSizeGet
#define sp1kDosRsvPartActSet    DOS_RsvPartActSet
#define sp1kDosRsvPartWPSet     DOS_RsvPartWPSet

#define sp1kDosRamStsGet    DOS_DosRamStsGet
#define sp1kDosRamCfg       DOS_DosRamCfg

#define sp1kDosGetDiskSpace    DOS_GetDiskSpace

#define sp1kClusterSizeGet	DOS_ClusterSizeGet
#endif

#endif  /* __SP1K_DOS_API_H__ */
