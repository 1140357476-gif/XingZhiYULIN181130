/**************************************************************************
 *
 *        Copyright (c) 2008-2008 by Sunplus mMedia Inc., Ltd.
 *
 *  This software is copyrighted by and is the property of Sunplus
 *  mMedia Inc., Ltd. All rights are reserved by Sunplus mMedia
 *  Inc., Ltd. This software may only be used in accordance with the
 *  corresponding license agreement. Any unauthorized use, duplication,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of Sunplus mMedia Inc., Ltd.
 *
 *  Sunplus mMedia Inc., Ltd. reserves the right to modify this
 *  software without notice.
 *
 *  Sunplus mMedia Inc., Ltd.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 **************************************************************************/
/**
 * @file		mtd_general.h
 * @brief		Macros api for MTD driver
 * @author	Phil Lin
 * @since		2008-07-01
 * @date		2008-07-01
 */
#ifndef __MTD_GENERAL_H__
#define __MTD_GENERAL_H__
/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#if 1
#define MTD_NEW
#endif
#define MTC_NAND_TIMEOUT_SUPPORT			1

#if (MTC_NAND_TIMEOUT_SUPPORT)

/*Define the nand state machine time out*/
#define MTD_NAND_AUTO_CMD_TIMEOUT		((UINT16)10000)

/*Define wait nand ready time out*/
#define MTD_NAND_RDY_TIMEOUT			((UINT16)10000)

/*Define if reset nand after nand wait rdy timeout*/
#define MTD_NAND_TIMEOUT_RST				1

#endif /*MTC_NAND_TIMEOUT_SUPPORT*/

/*Define mtd driver DMA timeout*/
#define MTD_DMA_TIME_OUT		((UINT32)0x80000)

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
#endif /*__MTD_GENERAL_H__*/

