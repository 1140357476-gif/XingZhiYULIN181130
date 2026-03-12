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

#ifndef __DBG_STR_H__
#define __DBG_STR_H__
//=============================================================================
//Symbol
//=============================================================================
//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------
#define SP1K_BANK_STRING_DEF		11
//------------------------------------------------------------------------------
//TYPE DEFINE
//------------------------------------------------------------------------------
typedef enum {
	CSTR_ERROR, 				
	CSTR_SUCCESS, 			
	CSTR_FAIL, 					
	CSTR_CMD_UI_HELP, 		
	CSTR_CMD_MEM_HELP, 		
	CSTR_CMD_ADC_HELP, 		
	CSTR_CMD_ADC_TEST_S,		
	CSTR_CMD_ADC_TEST_E, 	
	CSTR_CMD_ADC_TAST_START,
	CSTR_CMD_ADC_TEST_INFO,	
	CSTR_CMD_BTN_TEST_INFO,	
	CSTR_CMD_BTN_TEST_NOERR, 
	CSTR_CMD_BTN_TEST_RETRY, 	
} ConstStringList;

typedef struct tagString {
	ConstStringList strId;
	UINT8 code* str;
} ConstString_t;

//------------------------------------------------------------------------------
//Function Prototype
//------------------------------------------------------------------------------

void dbgPrintf(UINT16 StringID);
void dbgFuncTrack(UINT16 __FILE_ID, UINT16 __LINE_NUM) ;

#endif /*__DBG_STR_H__*/


