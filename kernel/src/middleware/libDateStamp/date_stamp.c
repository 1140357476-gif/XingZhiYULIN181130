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
#include "smcfunc.h"
#include "initio.h"
#include "hal.h"
#include "hal_jpeg.h"
#include "hal_gprm.h"
#include "sp1k_rsvblk_api.h"
#include "sp1k_util_api.h"
#include "sp1k_pv_api.h"
#include "util_api.h"
#include "dbg_def.h"
#include "video_clip.h"
#include "hal_global.h"
#include "video_mem.h"

//=============================================================================
//Symbol
//=============================================================================
//-----------------------------------------------------------------------------
//Constant
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//Variable
//-----------------------------------------------------------------------------

//xdata dateStc_t videoDateInf;

//xdata UINT8 dsCustomStr[DATE_STAMP_CUSTOM_STR_MAXSIZE] = {0};
xdata UINT16 dsParam[SP1K_DS_PARAM_MAX] = {0};
#if 0
extern UINT16 g_vidParam[VD_PARAM_MAX]; //wangrf++ 1.13

extern xdata UINT32 dsAddr1;
extern xdata UINT32 dsAddr2;
extern xdata UINT32 dsAddr3;
extern xdata UINT32 dsAddr4;
extern xdata UINT32 dsAddr5;
extern xdata UINT32 dsAddr6;
extern xdata UINT32 dsAddr7;
extern xdata videoStampCfg_t g_vidDateStamp;


#define __FILE	__FILE_ID_DATESTAMP__

//=============================================================================
//Program
//=============================================================================
//static void dateStampDateSet(UINT8 *str, UINT32 srcAddr, UINT32 dtnAddr, UINT16 dtnHsize, UINT16 dtnVsize, UINT16 dtnHoffSet, UINT16 dtnVoffSet, UINT16 dateHSize, UINT16 dateVSize, UINT16 dsVOffset, UINT16 dsVSize );
static void dateStampDateSet(UINT8 *str, UINT32 srcAddr, UINT32 dtnAddr, UINT16 dtnHsize, UINT16 dtnVsize, UINT16 dtnHoffSet, UINT16 dtnVoffSet, UINT16 dateHSize, UINT16 dateVSize);

void
dateStampPrepare(
	UINT32 addr,
	UINT32 tmpAddr,
	UINT16 dateHSize,
	UINT16 dateVSize
)
{
#if 0
	if( dateHSize >= DATE_STAMP_H_SIZE_1 && dateVSize >= DATE_STAMP_V_SIZE_1 ) {
		HAL_GprmScale(dsAddr1, (UINT16)DATE_STAMP_H_SIZE_1 * (UINT16)DATE_STAMP_TEMP_NUM, (UINT16)DATE_STAMP_V_SIZE_1,
			addr, (UINT16)dateHSize * (UINT16)DATE_STAMP_TEMP_NUM , (UINT16)dateVSize, tmpAddr, 1);
	}
	else if( dateHSize >= DATE_STAMP_H_SIZE_2 && dateVSize >= DATE_STAMP_V_SIZE_2 ) {
		HAL_GprmScale(dsAddr2, (UINT16)DATE_STAMP_H_SIZE_2 * (UINT16)DATE_STAMP_TEMP_NUM, (UINT16)DATE_STAMP_V_SIZE_2,
			addr, (UINT16)dateHSize * (UINT16)DATE_STAMP_TEMP_NUM , (UINT16)dateVSize, tmpAddr, 1);
	}
	else if( dateHSize >= DATE_STAMP_H_SIZE_3 && dateVSize >= DATE_STAMP_V_SIZE_3 ) {
		HAL_GprmScale(dsAddr3, (UINT16)DATE_STAMP_H_SIZE_3 * (UINT16)DATE_STAMP_TEMP_NUM, (UINT16)DATE_STAMP_V_SIZE_3,
			addr, (UINT16)dateHSize * (UINT16)DATE_STAMP_TEMP_NUM , (UINT16)dateVSize, tmpAddr, 1);
	}
	else if( dateHSize >= DATE_STAMP_H_SIZE_4 && dateVSize >= DATE_STAMP_V_SIZE_4 ) {
		HAL_GprmScale(dsAddr4, (UINT16)DATE_STAMP_H_SIZE_4 * (UINT16)DATE_STAMP_TEMP_NUM, (UINT16)DATE_STAMP_V_SIZE_4,
			addr, (UINT16)dateHSize * (UINT16)DATE_STAMP_TEMP_NUM , (UINT16)dateVSize, tmpAddr, 1);
	}
	else if( dateHSize >= DATE_STAMP_H_SIZE_5 && dateVSize >= DATE_STAMP_V_SIZE_5 ) {
		HAL_GprmScale(dsAddr5, (UINT16)DATE_STAMP_H_SIZE_5 * (UINT16)DATE_STAMP_TEMP_NUM, (UINT16)DATE_STAMP_V_SIZE_5,
			addr, (UINT16)dateHSize * (UINT16)DATE_STAMP_TEMP_NUM , (UINT16)dateVSize, tmpAddr, 1);
	}
	else if( dateHSize >= DATE_STAMP_H_SIZE_6 && dateVSize >= DATE_STAMP_V_SIZE_6 ) {
		HAL_GprmScale(dsAddr6, (UINT16)DATE_STAMP_H_SIZE_6 * (UINT16)DATE_STAMP_TEMP_NUM, (UINT16)DATE_STAMP_V_SIZE_6,
			addr, (UINT16)dateHSize * (UINT16)DATE_STAMP_TEMP_NUM , (UINT16)dateVSize, tmpAddr, 1);
	}
	else if( dateHSize >= DATE_STAMP_H_SIZE_7 && dateVSize >= DATE_STAMP_V_SIZE_7 ) {
		HAL_GprmScale(dsAddr7, (UINT16)DATE_STAMP_H_SIZE_7 * (UINT16)DATE_STAMP_TEMP_NUM, (UINT16)DATE_STAMP_V_SIZE_7,
			addr, (UINT16)dateHSize * (UINT16)DATE_STAMP_TEMP_NUM , (UINT16)dateVSize, tmpAddr, 1);
	}
	else {
		HAL_GprmScale(dsAddr7, (UINT16)DATE_STAMP_H_SIZE_7 * (UINT16)DATE_STAMP_TEMP_NUM, (UINT16)DATE_STAMP_V_SIZE_7,
			addr, (UINT16)dateHSize * (UINT16)DATE_STAMP_TEMP_NUM , (UINT16)dateVSize, tmpAddr, 1);
	}
#endif
}

//-----------------------------------------------------------------------------
//dateStampDateSet
//-----------------------------------------------------------------------------


/**
 * @fn        void dateStampDateSet(UINT8 *str, UINT32 dtAddr, UINT32 dtnAddr, UINT16 dtnHsize, UINT16 dtnVsize, UINT16 dtnHoffSet, UINT16 dtnVoffSet, UINT16 dateHSize, UINT16 dateVSize, UINT16 dsVOffset, UINT16 dsVSize)
 * @brief     copy date yuv to destination addr
 * @param     [in] str
 * @param     [in] dtAddr
 * @param     [in] dtnAddr  : destination address
 * @param     [in] dtnHsize  : destination hsize
 * @param     [in] dtnVsize  : destination vsize
 * @param     [in] dtnHoffSet  : destination h offset
 * @param     [in] dtnVoffSet  : destination v offset
 * @param     [in] dateHSize
 * @param     [in] dateVSize
 * @param     [in] dsVOffset
 * @param     [in] dsVSize
 * @retval    NONE
 * @see       NULL
 * @author    sun yong
 * @since     2010-7-7
 * @todo      N/A
 * @bug       N/A
*/
static void
dateStampDateSet(
	UINT8 *str,
	UINT32 srcAddr,
	UINT32 dtnAddr,
	UINT16 dtnHsize,
	UINT16 dtnVsize,
	UINT16 dtnHoffSet,
	UINT16 dtnVoffSet,
	UINT16 dateHSize,
	UINT16 dateVSize
)
{
#if 0
	UINT8 i;
	UINT8 val;
	UINT16 HoffSet;
	UINT16 dsnHoff;

	dsnHoff = dtnHoffSet;
	i = 0;
  
	while(str[i]!='\0') {
		val = str[i];//  '/' ~':' = 47 ~ 58

            if(val >= '/' && val <= ':'){
                val = val - '/'; //cont. chars
            }
            else {//xian ++ for date stamp string
                switch (val){                    
                    /*
                    case 'C':
                        val = ':' - '/' + 1;
                        break;
                        
                    case 'F':
                        val = ':' - '/' + 2;
                        break;
                    */    
                    case 'A':
                    	val = ':' - '/' + 1;
	                   	break;
					case 'P':
						val = ':' - '/' + 2;
						break;
					case 'M':
						val = ':' - '/' + 3;
						break;                    	
                    default:                    
                        val = DATE_STAMP_TEMP_NUM-1;   /*undefine char , it's space normally */              
                }
            }
                        
		HoffSet = (UINT16)val * dateHSize;

		HAL_GprmCopy(srcAddr, (UINT16)dateHSize* (UINT16)DATE_STAMP_TEMP_NUM, (UINT16)dateVSize, 
					HoffSet, 0,
					dtnAddr, dtnHsize, dtnVsize, 
					dsnHoff, dtnVoffSet,
					(UINT16)dateHSize, (UINT16)dateVSize, 					
					dsParam[SP1K_DS_PARAM_FONT_PASTETHD]/*0*//*(UINT16)0x80 + 0x3a*/, 1);   //xian modify 20090522
					
		i++;
		dsnHoff = dsnHoff + (UINT16)dateHSize;
	}
#endif
	return;
}



#if 1
/**
 * @brief		copy date yuv to destination addr
 * @param	Addr  : destination address
 * @param	hSize  : destination hsize
 * @param	vSize  : destination vsize
 * @param	xOffset  : destination h offset
 * @param	yOffset  : destination v offset
 * @param	mode  : bit[3~0]: 0: yyyy/mm/dd
 *\n                          1: dd/mm/yyyy
 *\n                          2: mm/dd/yyyy
 *\n                bit[5~4]: 0: no time
 *\n                          1: hh:mm
 *\n                          2: hh:mm am/pm
 *\n                          3: hh:mm:ss
 *\n                bit[7~6]: 0: no custom string
 *\n                          1: custom string with  date
 *\n                          2: custom string only 
 *\n    			bit[8]:   0: date and time one line show
 *\n						  1: date and time two line show	
 *\n    			bit[9]:   0: custom string is ahead "Date and Time"
 *\n						  1: custom string is behind "Data and Time"
 *\n    			bit[10]:  (only support one line mode) 
 *\n						  0: Date Time
 *\n						  1: Time Date
 
 
 * @retval	NONE
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @bug		NONE.
*/

UINT16
dateStampActive(
	dateStc_t dateInf,
	UINT32 srcAddr,
	UINT16 dateHSize,
	UINT16 dateVSize,	
	UINT32 dtnAddr,
	UINT16 dtnHSize,
	UINT16 dtnVSize,
	UINT16 dtnHOffset,
	UINT16 dtnVOffset,
	UINT16 mode
)
{
#if 0
	UINT8 date[32] ={0};
	UINT8 time[32] ={0};
	UINT16 dateLen, timeLen;
    UINT16 dsLen=0;
	UINT8 amFlag,amHour;
	
    //xian ++ for custom string
    if (mode&0xc0){
		
        dsLen = strlen( dsCustomStr );
        //printf("dsLen=%d\n",dsLen);
        ASSERT( (UINT32)dsLen * dateHSize + dtnHOffset < (UINT32)dtnHSize, 0 );


        if(mode==0x80){
            dateStampDateSet( dsCustomStr, srcAddr, dtnAddr, dtnHSize, dtnVSize, dtnHOffset, dtnVOffset, dateHSize, dateVSize);
            return dsLen;  // custom only
        }

		if ((mode & 0x200)==0){	/*string is the first*/
			dateStampDateSet( dsCustomStr, srcAddr, dtnAddr, dtnHSize, dtnVSize, dtnHOffset, dtnVOffset, dateHSize, dateVSize);
        	dtnHOffset = dtnHOffset + (UINT16)dateHSize * dsLen;
        }
    }

	//	HAL_GlobalReadRTC(&dateInf);
    switch( ( mode & 0x0f )) {
		case 0: // year/mon/day
			date[0] = '2';
			date[1] = '0';
			date[2] = dateInf.Year /10 + '0';
			date[3] = dateInf.Year %10 + '0';
			date[4] = '/';
			date[5] = dateInf.Month/10 + '0';
			date[6] = dateInf.Month%10 + '0';
			date[7] =  '/';
			date[8] = dateInf.Day/10 + '0';
			date[9] = dateInf.Day%10 + '0';
			date[10] = ' ';
			date[11] = '\0';

			break;

		case 1:
			date[0] = dateInf.Day/10 + '0';
			date[1] = dateInf.Day%10 + '0';
			date[2] =  '/';
			date[3] = dateInf.Month/10 + '0';
			date[4] = dateInf.Month%10 + '0';
			date[5] = '/';
			date[6] = '2';
			date[7] = '0';
			date[8] = dateInf.Year /10 + '0';
			date[9] = dateInf.Year %10 + '0';
			date[10] = ' ';
			date[11] = '\0';

			break;

		case 2:
			date[0] = dateInf.Month/10 + '0';
			date[1] = dateInf.Month%10 + '0';
			date[2] =  '/';
			date[3] = dateInf.Day/10 + '0';
			date[4] = dateInf.Day%10 + '0';
			date[5] = '/';
			date[6] = '2';
			date[7] = '0';
			date[8] = dateInf.Year /10 + '0';
			date[9] = dateInf.Year %10 + '0';
			date[10] = ' ';
			date[11] = '\0';
			break;

		default:
			date[0] = '2';
			date[1] = '0';
			date[2] = dateInf.Year /10 + '0';
			date[3] = dateInf.Year %10 + '0';
			date[4] = '/';
			date[5] = dateInf.Month/10 + '0';
			date[6] = dateInf.Month%10 + '0';
			date[7] =  '/';
			date[8] = dateInf.Day/10 + '0';
			date[9] = dateInf.Day%10 + '0';
			date[10] = ' ';
			date[11] = '\0';

			
    		time[0] = dateInf.Hour/10 +'0';
    		time[1] = dateInf.Hour%10 +'0';
    		time[2] = ':';
    		time[3] = dateInf.Minute/10 +'0';
    		time[4] = dateInf.Minute%10 +'0';

			break;
	}

    if(mode&0x30){
		amHour = dateInf.Hour ; //xian ++ 20110607, #42760
    	if((mode&0x30)==0x20){
    		if(dateInf.Hour>12){
				amHour = dateInf.Hour - 12;
				amFlag=0;
    		}
    		else{
    			amFlag=1;
			}			
		}

		time[0] = amHour/10 +'0';
		time[1] = amHour%10 +'0';			
		time[2] = ':';
		time[3] = dateInf.Minute/10 +'0';
		time[4] = dateInf.Minute%10 +'0';
		time[5] = ' ';

		if((mode&0x30)==0x30){
		    time[5] = ':';
    		time[6] = dateInf.Second/10 +'0';
    		time[7] = dateInf.Second%10 +'0';
    		time[8] = ' ';		
		}     
		else if((mode&0x20)==0x20){
			if(amFlag==1){
		    	time[5] = 'A';
		    }
		    else{
		    	time[5] = 'P';
		    }
		    
    		time[6] = 'M';
    		time[7] = ' ';
    		time[8] = '\0';		
		}

    }

	dateLen = strlen( date );	
	timeLen = strlen( time );
	dsLen += (dateLen + timeLen);//wangrf edit
	//printf("dateLen=%d\n",dateLen);
	//printf("timeLen=%d\n",timeLen);
	//printf("dsLen=%d\n",dsLen);
	
	
		//printf("dtnHSize=%d\n",dtnHSize);
	if( ( mode & 0x30 ) != 0x00 ) {
		//ASSERT( (UINT32)( dateLen + timeLen + 1 ) * dateHSize + dtnHOffset < (UINT32)dtnHSize, 0 );
        if(( mode & 0x100 ) == 0x100 ) //date and time two line show
        {         
			ASSERT( (UINT32)dateLen * dateHSize + dtnHOffset <= (UINT32)dtnHSize, 0 );
			//dtnVOffset =0;
			dateStampDateSet( time, srcAddr, dtnAddr, dtnHSize, dtnVSize, dtnHOffset, dtnVOffset, dateHSize, dateVSize );
			dtnVOffset = dtnVOffset - dateVSize ;  
			dtnAddr=dtnAddr;
			dateStampDateSet( date, srcAddr,dtnAddr, dtnHSize, dtnVSize, dtnHOffset, dtnVOffset, dateHSize, dateVSize );		
			dtnHOffset = dtnHOffset + (UINT16)dateHSize * dateLen;  
        }
		else
		{		
		    ASSERT( (UINT32)dsLen * dateHSize + dtnHOffset <= (UINT32)dtnHSize, 0 );

		    if (mode & 0x400){
				dateStampDateSet( time, srcAddr, dtnAddr, dtnHSize, dtnVSize, dtnHOffset, dtnVOffset, dateHSize, dateVSize );
				dtnHOffset = dtnHOffset + (UINT16)dateHSize *(timeLen);  
				dateStampDateSet( date, srcAddr, dtnAddr, dtnHSize, dtnVSize, dtnHOffset, dtnVOffset, dateHSize, dateVSize );
				dtnHOffset = dtnHOffset + (UINT16)dateHSize * dateLen;	
		    }
		    else{
				dateStampDateSet( date, srcAddr, dtnAddr, dtnHSize, dtnVSize, dtnHOffset, dtnVOffset, dateHSize, dateVSize );
				dtnHOffset = dtnHOffset + (UINT16)dateHSize * dateLen;  
				dateStampDateSet( time, srcAddr, dtnAddr, dtnHSize, dtnVSize, dtnHOffset, dtnVOffset, dateHSize, dateVSize );
				dtnHOffset = dtnHOffset + (UINT16)dateHSize *(timeLen);  
			}
		}

	}
	else {
		ASSERT( (UINT32)dateLen * dateHSize + dtnHOffset < (UINT32)dtnHSize, 0 );
		
		//dtnHOffset = dtnHOffset + (UINT16)dateHSize * 6;
		dateStampDateSet( date, srcAddr, dtnAddr, dtnHSize, dtnVSize, dtnHOffset, dtnVOffset, dateHSize, dateVSize );
		dtnHOffset = dtnHOffset + (UINT16)dateHSize *dateLen;  
	}


	if ((mode & 0x240)==0x240){	/*string is the last*/
		dateStampDateSet( dsCustomStr, srcAddr, dtnAddr, dtnHSize, dtnVSize, dtnHOffset, dtnVOffset, dateHSize, dateVSize); 		
	}

	return dsLen;
	#endif
}

#else
//-----------------------------------------------------------------------------
//dateStampDateSet
//-----------------------------------------------------------------------------
/**
 * @brief		copy date yuv to destination addr
 * @param	Addr  : destination address
 * @param	hSize  : destination hsize
 * @param	vSize  : destination vsize
 * @param	xOffset  : destination h offset
 * @param	yOffset  : destination v offset
 * @param	mode  : bit[3~0]: 0: yyyy/mm/dd
 *\n                          1: dd/mm/yyyy
 *\n                          2: mm/dd/yyyy
 *\n                bit[5~4]: 0: no time
 *\n                          1: hh:mm
 *\n                          3: hh:mm:dd
 *\n                bit[7~6]: 0: no custom string
 *\n                          1: custom string with  date
 *\n                          2: custom string only 
 * @retval	NONE
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @bug		NONE.
*/

void
dateStampActive(
	dateStc_t dateInf,
	UINT32 srcAddr,
	UINT32 dtnAddr,
	UINT16 hSize,
	UINT16 vSize,
	UINT16 hOffset,
	UINT16 vOffset,
	UINT16 dateHSize,
	UINT16 dateVSize,
	UINT16 dsVOffset,
	UINT16 dsVSize,
	UINT8 mode
)
{
	UINT8 date[32] ={0};
	UINT8 time[32] ={0};
	UINT32 dateLen, timeLen;
    UINT8 strLen;

    //xian ++ for custom string
    if (mode&0xc0){
        strLen = strlen( dsCustomStr );
        ASSERT( (UINT32)strLen * dateHSize + hOffset < (UINT32)hSize, 0 );
        dateStampDateSet( dsCustomStr, srcAddr, dtnAddr, hSize, vSize, hOffset, vOffset, dateHSize, dateVSize, dsVOffset, dsVSize );

        if(mode==0x80)
            return; // custom only

        hOffset = hOffset + (UINT16)dateHSize * strLen;
    }

	//	HAL_GlobalReadRTC(&dateInf);
    switch( ( mode & 0x0f )) {
		case 0: // year/mon/day
			date[0] = '2';
			date[1] = '0';
			date[2] = dateInf.Year /10 + '0';
			date[3] = dateInf.Year %10 + '0';
			date[4] = '/';
			date[5] = dateInf.Month/10 + '0';
			date[6] = dateInf.Month%10 + '0';
			date[7] =  '/';
			date[8] = dateInf.Day/10 + '0';
			date[9] = dateInf.Day%10 + '0';

			break;

		case 1:
			date[0] = dateInf.Day/10 + '0';
			date[1] = dateInf.Day%10 + '0';
			date[2] =  '/';
			date[3] = dateInf.Month/10 + '0';
			date[4] = dateInf.Month%10 + '0';
			date[5] = '/';
			date[6] = '2';
			date[7] = '0';
			date[8] = dateInf.Year /10 + '0';
			date[9] = dateInf.Year %10 + '0';

			break;

		case 2:
			date[0] = dateInf.Month/10 + '0';
			date[1] = dateInf.Month%10 + '0';
			date[2] =  '/';
			date[3] = dateInf.Day/10 + '0';
			date[4] = dateInf.Day%10 + '0';
			date[5] = '/';
			date[6] = '2';
			date[7] = '0';
			date[8] = dateInf.Year /10 + '0';
			date[9] = dateInf.Year %10 + '0';

			break;

		default:
			date[0] = '2';
			date[1] = '0';
			date[2] = dateInf.Year /10 + '0';
			date[3] = dateInf.Year %10 + '0';
			date[4] = '/';
			date[5] = dateInf.Month/10 + '0';
			date[6] = dateInf.Month%10 + '0';
			date[7] =  '/';
			date[8] = dateInf.Day/10 + '0';
			date[9] = dateInf.Day%10 + '0';

    		time[0] = dateInf.Hour/10 +'0';
    		time[1] = dateInf.Hour%10 +'0';
    		time[2] = ':';
    		time[3] = dateInf.Minute/10 +'0';
    		time[4] = dateInf.Minute%10 +'0';

			break;
	}

    if(mode&0x30){
		time[0] = dateInf.Hour/10 +'0';
		time[1] = dateInf.Hour%10 +'0';
		time[2] = ':';
		time[3] = dateInf.Minute/10 +'0';
		time[4] = dateInf.Minute%10 +'0';

        if(mode&0x20){
		    time[5] = ':';
    		time[6] = dateInf.Second/10 +'0';
    		time[7] = dateInf.Second%10 +'0';
        }
    }

	dateLen = strlen( date );
	timeLen = strlen( time );

	if( ( mode & 0x10 ) == 0x10 ) {
		ASSERT( (UINT32)( dateLen + timeLen + 1 ) * dateHSize + hOffset < (UINT32)hSize, 0 );

		dateStampDateSet( date, srcAddr, dtnAddr, hSize, vSize, hOffset, vOffset, dateHSize, dateVSize, dsVOffset, dsVSize );

		hOffset = hOffset + (UINT16)dateHSize * 11;
		dateStampDateSet( time, srcAddr, dtnAddr, hSize, vSize, hOffset, vOffset, dateHSize, dateVSize, dsVOffset, dsVSize );
	}
	else {
		ASSERT( (UINT32)dateLen * dateHSize + hOffset < (UINT32)hSize, 0 );

		hOffset = hOffset + (UINT16)dateHSize * 6;

		dateStampDateSet( date, srcAddr, dtnAddr, hSize, vSize, hOffset, vOffset, dateHSize, dateVSize, dsVOffset, dsVSize );
	}

	return;
}
#endif
#endif
