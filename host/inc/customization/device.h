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
 * @file		device.h
 * @brief		H/W device difinition
 * @author		Phil Lin
 * @since		2008-02-20
 * @date		2008-08-19
 */

#ifndef __DEVICE_H__
#define __DEVICE_H__

#include "solution.h"

//-----------------------------------------------------------------------------
// Device type
//-----------------------------------------------------------------------------

/*
----------------------------------------------------------------------------
                                     DISK
----------------------------------------------------------------------------
*/

#define STROBE_DEVICE 0

/*
	The macro "LIB_RELEASE" might be defined in each lib project
*/
#ifndef LIB_RELEASE 

/* for FDK internal use */
#if 1
#define DISK_DRV_CARD
#endif

/* for FDK internal use */
#if 1
#define DISK_DRV_NAND
#endif

/* for FDK internal use */
#if 0
#define DISK_DRV_NOR
#endif

/* for FDK internal use */
#if 0
#define DISK_DRV_SPI
#endif

/* for FDK internal use */
#if 0
#define DISK_DRV_DRAM	
#endif

#else

/* lib release parsing */
#ifndef DISK_DRV_NAND
	#ifndef DISK_DRV_NOR
		#ifndef DISK_DRV_SPI
			#define DISK_DRV_NAND
		#endif
	#endif
#endif

/* for customer release */
#if 0
#define DISK_DRV_DRAM 
#endif

#endif // LIB_RELEASE

//-----------------------------------------------------------------------------
//Type definition
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//Function declare
//-----------------------------------------------------------------------------
UINT8 sp1kDevInfoUpdate(devId_t devId, devAttr_t* const pDevAttr);

/*
*used for swap the detection polarity of TV
*
*FALSE -> not swap 
*TRUE  -> swap
*
*Note:
	If power on camera with tv wire plugged in, some kit can not output the tv
	single .This because the polarity of tv detect single was mistaked
	by customer's layout engineer.In this case,we can only change the polarity
	of the single.
	
	This function should be called before INITIO_Initialize() in app_init.c
	because the static variable should be initialled before use.

	If this function not be called, this means the polarity keep no changed.
	(this is the default status).
*/
void sp1kTVDetPolaritySet(UINT8 Po);


/* macro expand.
 */
#include "macro_expand.h"

#endif /*__DEVICE_H__*/

