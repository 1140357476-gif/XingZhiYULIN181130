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
 *  Author: sunyong                                                *
 *                                                                        *
 **************************************************************************/
/**
 * @file		datestamp.c
 * @brief		sp1k API for RTC init.
 * @author	sun yong
 * @since		2008-03-01
 * @date		2008-03-01
 */
//=============================================================================
//Header file
//=============================================================================
#include "general.h"
#include "date_stamp.h"
#include "initio.h"
#include "sp1k_rsvblk_api.h"
#include "sp1k_util_api.h"
#include "sp1k_pv_api.h"
//=============================================================================
//Symbol
//=============================================================================
//-----------------------------------------------------------------------------
//Constant
//-----------------------------------------------------------------------------
#if 0
//-----------------------------------------------------------------------------
//Variable
//-----------------------------------------------------------------------------
xdata UINT32 dsAddr1;
xdata UINT32 dsAddr2;
xdata UINT32 dsAddr3;
xdata UINT32 dsAddr4;
xdata UINT32 dsAddr5;
xdata UINT32 dsAddr6;
xdata UINT32 dsAddr7;

extern xdata UINT8 dsCustomStr[DATE_STAMP_CUSTOM_STR_MAXSIZE];
extern xdata UINT16 dsParam[SP1K_DS_PARAM_MAX];
//=============================================================================
//Program
//=============================================================================

//-----------------------------------------------------------------------------
//sp1kLoadDateStampData
//-----------------------------------------------------------------------------
/**
 * @brief		load date stamp yuv for nand
 * @param	smallSize : 0 big size, 1 normal size , 2 small size
 * @param	tmpAddr  : if smallsize is 1 or 2,the tmpaddress must be used when scaling
 * @retval	NONE
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @bug		NONE.
*/

void sp1kLoadDateStampData(void)
{
	UINT32 size1, size2, size3, size4, size5, size6, size7;
	UINT32 endAddr;

	size1 = (UINT32)DATE_STAMP_H_SIZE_1 * DATE_STAMP_V_SIZE_1 * DATE_STAMP_TEMP_NUM * 2;
	size2 = (UINT32)DATE_STAMP_H_SIZE_2 * DATE_STAMP_V_SIZE_2 * DATE_STAMP_TEMP_NUM * 2;
	size3 = (UINT32)DATE_STAMP_H_SIZE_3 * DATE_STAMP_V_SIZE_3 * DATE_STAMP_TEMP_NUM * 2;
	size4 = (UINT32)DATE_STAMP_H_SIZE_4 * DATE_STAMP_V_SIZE_4 * DATE_STAMP_TEMP_NUM * 2;
	size5 = (UINT32)DATE_STAMP_H_SIZE_5 * DATE_STAMP_V_SIZE_5 * DATE_STAMP_TEMP_NUM * 2;
	size6 = (UINT32)DATE_STAMP_H_SIZE_6 * DATE_STAMP_V_SIZE_6 * DATE_STAMP_TEMP_NUM * 2;
	size7 = (UINT32)DATE_STAMP_H_SIZE_7 * DATE_STAMP_V_SIZE_7 * DATE_STAMP_TEMP_NUM * 2;

	dsAddr1 = K_SDRAM_GrafFontBufAddr;
	dsAddr1 = ( dsAddr1 + 15 ) >> 4 << 4;

	dsAddr2 = dsAddr1 + ( size1 >> 1 );
	dsAddr2 = ( dsAddr2 + 15 ) >> 4 << 4;

	dsAddr3 = dsAddr2 + ( size2 >> 1 );
	dsAddr3 = ( dsAddr3 + 15 ) >> 4 << 4;

	dsAddr4 = dsAddr3 + ( size3 >> 1 );
	dsAddr4 = ( dsAddr4 + 15 ) >> 4 << 4;

	dsAddr5 = dsAddr4 + ( size4 >> 1 );
	dsAddr5 = ( dsAddr5 + 15 ) >> 4 << 4;

	dsAddr6 = dsAddr5 + ( size5 >> 1 );
	dsAddr6 = ( dsAddr6 + 15 ) >> 4 << 4;

	dsAddr7 = dsAddr6 + ( size6 >> 1 );
	dsAddr7 = ( dsAddr7 + 15 ) >> 4 << 4;

	endAddr = dsAddr7 + ( size7 >> 1 );

	if( endAddr - K_SDRAM_GrafFontBufAddr > K_SDRAM_GrafFontBufSize ) {
		printf( "%s: %ld, DS size too big\n", __FILE__, (UINT32)__LINE__ );

		while( 1 );
	}

	sp1kNandRsvRead(0x23, (dsAddr1<< 1));
	sp1kNandRsvRead(0x24, (dsAddr2<< 1));
	sp1kNandRsvRead(0x25, (dsAddr3<< 1));
	sp1kNandRsvRead(0x26, (dsAddr4<< 1));
	sp1kNandRsvRead(0x27, (dsAddr5<< 1));
	sp1kNandRsvRead(0x28, (dsAddr6<< 1));
	sp1kNandRsvRead(0x29, (dsAddr7<< 1));

	//dramFileSave("DATERSV0.YUV",dsAddr1,size1);
}

void sp1kDateStampCustomStrSet(UINT8 *str)
{
    UINT8 i;

    for(i=0; i<DATE_STAMP_CUSTOM_STR_MAXSIZE-1; i ++){
        if(*str){
            dsCustomStr[i] = *str;
            str ++;
        }
        else 
            break;
    }
    dsCustomStr[i] = '\0';
}

//xian ++ 20090522
void sp1kDateStampParamSet(UINT8 paraId, UINT16 value)
{
	if (paraId >= SP1K_DS_PARAM_MAX) {
		return;
	}

    dsParam[paraId] = value;
}
UINT16 sp1kDateStampParamGet(UINT8 paraId)
{
	if (paraId >= SP1K_DS_PARAM_MAX) {
		return FAIL;
	}

	return dsParam[paraId];
}
//xian --
#endif
