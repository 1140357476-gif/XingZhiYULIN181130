/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2007 by Sunplus mMedia Inc.                      *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus        *
 *  mMedia Inc. All rights are reserved by Sunplus mMedia Inc.  This      *
 *  software may only be used in accordance with the corresponding        *
 *  license agreement. Any unauthorized use, duplication, distribution,   *
 *  or disclosure of this software is expressly forbidden.                *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus mMedia Inc.                                *
 *                                                                        *
 *  Sunplus mMedia Inc. reserves the right to modify this software        *
 *  without notice.                                                       *
 *                                                                        *
 *  Sunplus mMedia Inc.                                                   *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 **************************************************************************/

/**
 * @file		solution.h
 * @brief		platform selection
 * @author	Phil Lin
 * @since		2008-02-20
 * @date		2008-02-20
 */
#ifndef __SOLUTION_H__
#define __SOLUTION_H__
 //=============================================================================
//Header file
//=============================================================================

//=============================================================================
//Symbol
//=============================================================================
//-----------------------------------------------------------------------------
//Constant
//-----------------------------------------------------------------------------
/*
----------------------------------------------------------------------------
                                                         HW_SOLUTION
----------------------------------------------------------------------------
*/
/*
H/W definition
*/
#define _HW_EVB_				0x00
#define _HW_TK_MODEL_A_			0x01
#define _HW_TK_MODEL_B_			0x02
#define _HW_RDK_A_				0x03

#define _HW_CUSTOM_MODEL_A_		0x10
#define _HW_CUSTOM_MODEL_B_		0x11

/*
H/W selection
*/

#ifndef _HW_SOLUTION_

#if 1
#define _HW_SOLUTION_		_HW_EVB_
#endif

#if 0
#define _HW_SOLUTION_		_HW_CUSTOM_MODEL_A_
#endif

#if 0
#define _HW_SOLUTION_		_HW_CUSTOM_MODEL_B_
#endif

#if 0
#define _HW_SOLUTION_		_HW_RDK_A_
#endif
 
#endif //_HW_SOLUTION_
#endif /*__SOLUTION_H__*/

