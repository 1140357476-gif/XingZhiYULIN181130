/**************************************************************************
 *         Copyright(c) 2007 by Sunplus mMedia  Technology Co., Ltd.       *
 *                                                                         *
 *  This software is copyrighted by and is the property of Sunplus mMedia  *
 *  Technology Co., Ltd. All rights are reserved by Sunplus mMedia Techno- *
 *  logy Co., Ltd. This software may only be used in accordance with the   *
 *  corresponding license agreement. Any unauthorized use, duplication,    *
 *  distribution, or disclosure of this software is expressly forbidden.   *
 *                                                                         *
 *  This Copyright notice "M U S T" not be removed or modified without     *
 *  prior written consent of Sunplus mMedia Technology  Co., Ltd.          *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd. reserves the right to modify this  *
 *  software without notice.                                               *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd.                                    *
 *  19-1, Innovation First Road, Science-Based Industrial Park,            *
 *  Hsin-Chu, Taiwan, R.O.C.                                               *
 *                                                                         *
 **************************************************************************/
#ifndef __SP1K_NANDRSV_API_H__
#define __SP1K_NANDRSV_API_H__

/** \file sp1k_rsvblk_api.h
 * @brief		Spca1628 reserved data access related interface
 */

#include "sp1k_def.h"
#include "Hal_device.h"
#include "smcfunc.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#ifdef SP1K_DOXY_GEN
/**
 * @fn UINT8 sp1kNandRsvRead(UINT32 idx, UINT32 addr)
 * @brief	To read the reserve data from nand by index and dram buffer address
 * @param[in] id    specify the reserve file index
 * @param[in] addr  specify the dram buffer address ( byte ) for data read back from nand
 * @retval\n 
 *      TRUE  : read error.
 *\n    FALSE : read success                               
 * @see sp1kNandRsvWrite(), sp1kNandRsvSizeGet()
*/
UINT8 sp1kNandRsvRead(UINT32 idx, UINT32 addr);
/**
 * @fn UINT8 sp1kNandRsvWrite(UINT32 idx, UINT32 addr)
 * @brief	Write the reserve data to nand by index and dram buffer address
 * @param[in] idx   specify the reserve file index
 * @param[in] addr  specify the dram buffer address ( byte ) for writing data into nand
 * @retval\n 
 *      TRUE  : read error.
 *\n    FALSE : read success                               
 * @see sp1kNandRsvRead(), sp1kNandRsvSizeGet()
 */
UINT8 sp1kNandRsvWrite(UINT32 idx, UINT32 addr);
/**
 * @fn UINT8 sp1kNandRsvSizeGet(UINT32 idx, UINT32 *offset, UINT32 *size)
 * @brief	To query the offset and size of the target index
 * @param[in]  idx	   specify the reserve file index
 * @param[out] *offset The offset value of current resource by index 
 * @param[out] *size   The size ( bytes ) of current resource by index
 * @retval\n
 *      TRUE  : query error.
 *\n    FALSE : query success                               
 * @see sp1kNandRsvRead(), sp1kNandRsvWrite()
 */
UINT8 sp1kNandRsvSizeGet(UINT32 idx, UINT32 *offset, UINT32 *size);
#else

#define sp1kNandRsvRead      sp1kDiskRsvRead
#define sp1kNandRsvWrite     sp1kDiskRsvWrite
#define sp1kNandRsvSizeGet   sp1kDiskRsvSizeGet

#endif

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
#endif //__SP1K_NANDRSV_API_H__

