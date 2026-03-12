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
 * @file		RTC.c
 * @brief		sp1k API for RTC init.
 * @author	sun yong
 * @since		2008-03-01
 * @date		2008-03-01
 */
//=============================================================================
//Header file
//=============================================================================
#include "general.h"
#include "hal.h"
#include "hal_global.h"

//=============================================================================
//Symbol
//=============================================================================
//-----------------------------------------------------------------------------
//Constant
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//Variable
//-----------------------------------------------------------------------------
xdata dateStc_t	DATEINFO;
//=============================================================================
//Program       
//=============================================================================
//-----------------------------------------------------------------------------
//sp1kRTCDateInit
//-----------------------------------------------------------------------------
/**
 * @brief		Init RTC Date .
 * @param	NONE
 * @retval	NONE
 * @see
 * @author	kai.wang
 * @since		2011-05-16
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kRTCDateInit(pDateStc_t DatePt)
{
	
	DATEINFO.Year = DatePt->Year;
	DATEINFO.Month = DatePt->Month;
	DATEINFO.Day = DatePt->Day;
	DATEINFO.Hour = DatePt->Hour;
	DATEINFO.Minute = DatePt->Minute;
	DATEINFO.Second = DatePt->Second;
}

//-----------------------------------------------------------------------------
//sp1kRTCInit
//-----------------------------------------------------------------------------
/**
 * @brief		sp1k RTC init.
 * @param	NONE
 * @retval	NONE
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/

void sp1kRTCInit(void) 
{
	UINT8 reliableData;	

	HAL_GlobalRTCDrivingSet(0x02); //set RTC Driving @1.5uA
	HAL_GlobalReadRTCData(0x02, &reliableData);

        if (reliableData != 0x5a)
        {
        	HAL_GlobalWriteRTC(&DATEINFO);
        }
		
	//HAL_GlobalReadRTC(&DATEINFO);
	//printf("Time: 20%02bd, %02bd, %02bd\n",DATEINFO.Year, DATEINFO.Month, DATEINFO.Day); 
       //printf("%02bd : %02bd : %02bd\n",DATEINFO.Hour, DATEINFO.Minute, DATEINFO.Second); 
	
	return;	
}		


