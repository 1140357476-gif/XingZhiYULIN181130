/**************************************************************************
 *		   Copyright(c) 2010 by i-Catch Technology Co., Ltd.			  *
 *																		  *
 *	This software is copyrighted by and is the property of i-Catch Tech-  *
 *	nology Co., Ltd. All rights are reserved by i-Catch Technology Co.,   *
 *	Ltd. This software may only be used in accordance with the			  *
 *	corresponding license agreement. Any unauthorized use, duplication,   *
 *	distribution, or disclosure of this software is expressly forbidden.  *
 *																		  *
 *	This Copyright notice "M U S T" not be removed or modified without	  *
 *	prior written consent of i-Catch Technology Co., Ltd.				  *
 *																		  *
 *	i-Catch Technology Co., Ltd. reserves the right to modify this		  *
 *	software without notice.											  *
 *																		  *
 *	i-Catch Technology Co., Ltd.										  *
 *	19-1, Innovation First Road, Science-Based Industrial Park, 		  *
 *	Hsin-Chu, Taiwan, R.O.C.											  *
 *																		  *
 *************************************************************************/

/**
 * @file		hal_global.c
 * @brief		Hal API for global module
 * @author		kai.wang
 * @since		2010-01-01
 * @date		2010-01-01
 */

/**************************************************************************
 *						  H E A D E R	F I L E S
 **************************************************************************/
#include "general.h"
#include "hal.h"
#include "main.h"
#include "hal_gpio.h"
#include "hal_global.h"
#include "reg_def.h"

/**************************************************************************
 *							 C O N S T A N T S							  *
 **************************************************************************/
 
/**************************************************************************
 *								M A C R O S 							  *
 **************************************************************************/
#define IC_VER_DETECT	1 //0:default;    1:auto detect 
/**************************************************************************
 *							D A T A    T Y P E S						  *
 **************************************************************************/
 
/**************************************************************************
 *						   G L O B A L	  D A T A						  *
 **************************************************************************/
static pFuncGT2 xdata GT2PFunc;
static UINT8* xdata GT2PFuncPara;
static UINT8 xdata rwRTCDataSts;
/**************************************************************************
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/
 
/**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/


//-----------------------------------------------------------------------------
//HAL_GlobalTimerInit
//-----------------------------------------------------------------------------
/**
 * @fn		  void HAL_GlobalTimerInit(void)
 * @brief	  Initialize global timer1
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-4-26
 * @todo	  N/A
 * @bug 	  N/A
*/
void HAL_GlobalTimerInit(void)
{
	XBYTE[0x20C5] = 0x01;	//up count mode and time stick: 10us

	XBYTE[0x20C1] = 0x01;	/* set initial value */
	XBYTE[0x20C2] = 0x00;
	XBYTE[0x20C3] = 0x00;
	XBYTE[0x20C4] = 0x00;
	XBYTE[0x20C6] |= 0x04;	/*load time*/

	//XBYTE[0x20D0] |= 0x04;	/* Global timer1 interrupt enable */
	XBYTE[0x20C6] = 0x01;	/* start timer1 */
}

//-----------------------------------------------------------------------------
//HAL_GlobalTimerRead10us
//-----------------------------------------------------------------------------
/**
 * @fn		  void HAL_GlobalTimerRead10us(UINT32* G10ustimerPt)
 * @brief	  Read the global timer1 value that is specified with the unit of 10 micro-second
 * @param	  [in] * GtimerPt: the value of global timer1
 * @retval	  NONE
 * @see 	  HAL_Read10usGTimer
 * @author	  kai.wang
 * @since	  2010-4-26
 * @todo	  N/A
 * @bug 	  N/A
*/
#pragma OT(8, SPEED)
void HAL_GlobalTimerRead10us(UINT32 xdata* G10ustimerPt) USING_0
{
	BIT mode;
	UINT32 val;

	mode = XBYTE[0x20C5] & 0x01;

	do {
		*((UINT8 xdata*)G10ustimerPt + 3) = XBYTE[0x20C1];
		*((UINT8 xdata*)G10ustimerPt + 2) = XBYTE[0x20C2];
		*((UINT8 xdata*)G10ustimerPt + 1) = XBYTE[0x20C3];
		*((UINT8 xdata*)G10ustimerPt + 0) = XBYTE[0x20C4];
		
		WRITE8(val, 0, XBYTE[0x20C4]);
		WRITE8(val, 1, XBYTE[0x20C3]);
		WRITE8(val, 2, XBYTE[0x20C2]);
		WRITE8(val, 3, XBYTE[0x20C1]);
	} while(mode ? *G10ustimerPt > val : *G10ustimerPt < val);
}

//-----------------------------------------------------------------------------
//HAL_GlobalReadGTimer
//-----------------------------------------------------------------------------
/**
 * @fn		  void HAL_GlobalReadGTimer(UINT32* GtimerPt)
 * @brief	  Read the global timer value that is specified with the unit of mini-second
 * @param	  [in] * GtimerPt: the value of global timer1
 * @retval	  NONE
 * @see 	  HAL_GlobalTimerRead10us
 * @author	  kai.wang
 * @since	  2010-4-26
 * @todo	  N/A
 * @bug 	  N/A
*/
void HAL_GlobalReadGTimer(UINT32* GtimerPt) USING_0
{
	UINT32 usTime;
	HAL_GlobalTimerRead10us(& usTime);
	*GtimerPt = usTime / 100 ;
	return;
}

//-----------------------------------------------------------------------------
//HAL_10usWait
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 HAL_10usWait(UINT8 WaitTime)
 * @brief	  Pause the firmware execution by calling a waiting a period of time. The
			timing is calculated by the up-count mode of the global timer1.
 * @param	  [in] WaitTime: The wait time that is specified by the unit of 10 micro-second.
 * @retval	  return = SUCCESS / FAIL
 * @see 	  HAL_10usWait
 * @author	  kai.wang
 * @since	  2010-4-26
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 HAL_GlobalTimerWait10us(UINT8 WaitTime) USING_0
{
	UINT8 status = HALK_SUCCESS;

	UINT8 pre =0,post = 0, sub = 0;
	
	pre = XBYTE[0x20c1];

	do
	{
		post = XBYTE[0x20c1];
		if (post < pre){
			sub = 0xFF - pre + post + 1;
		}
		else {
			sub = post - pre;
		}
	} while(sub < WaitTime);

	return status;
}
//-----------------------------------------------------------------------------
//HAL_GT2_10usWait
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 HAL_GT2_10usWait(UINT8 WaitTime)
 * @brief	  Pause the firmware execution by calling a waiting a period of time. The
			timing is calculated by the up-count mode of the global timer2.
 * @param	  [in] WaitTime: The wait time that is specified by the unit of 10 micro-second.
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-4-26
 * @todo	  N/A
 * @bug 	  N/A

UINT8 HAL_GlobalTimer2Wait10us(UINT8 WaitTime) USING_0
{
	UINT8 status = HALK_SUCCESS;

	UINT8 pre =0,post = 0, sub = 0;
	/* for FPGA use 
	#if (FPGA_VERIFY)
	WaitTime = WaitTime /13 + 1; //65.25/5=13.05
	#endif

	pre = XBYTE[0x20C7];

	do
	{
		post = XBYTE[0x20C7];
		if (post < pre){
			sub = 0xFF - pre + post;
		}
		else {
			sub = post - pre;
		}
	} while(sub < WaitTime);

	return status;
}*/

//-----------------------------------------------------------------------------
//HAL_Wait
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 HAL_Wait(UINT32 WaitTime)
 * @brief	  Pause the firmware execution by calling a waiting a period of time. The
			timing is calculated by the up-count mode of the global timer1.
 * @param	  [in] WaitTime: The wait time that is specified by the unit of mini-second.
 * @retval	  return = SUCCESS / FAIL
 * @see 	  HAL_Wait
 * @author	  kai.wang
 * @since	  2010-4-26
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 HAL_GlobalTimerWait(UINT32 WaitTime) USING_0
{
	UINT8 status;
	UINT32 i;

	if(WaitTime<=0xffffff)
	{
		for(i=0; i<WaitTime; i++)
		HAL_GlobalTimerWait10us(100);
	}
	else
	{
		status = HALK_ERROR_PARAMETER;
	}

	return(status);
}
//-----------------------------------------------------------------------------
//HAL_GT2_Wait
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 HAL_GT2_Wait(UINT32 WaitTime)
 * @brief	  Pause the firmware execution by calling a waiting a period of time. The
			timing is calculated by the up-count mode of the global timer.
 * @param	  [in] WaitTime: The wait time that is specified by the unit of mini-second.
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-4-26
 * @todo	  N/A
 * @bug 	  N/A

UINT8 HAL_GlobalTimer2Wait(UINT32 WaitTime) USING_0
{
	UINT8 status;
	UINT32 i;

	if(WaitTime<=0xffffff)
	{
		for(i=0; i<WaitTime; i++)
		HAL_GlobalTimerWait10us(100);
	}
	else
	{
		status = HALK_ERROR_PARAMETER;
	}

	return(status);
}
*/
//-----------------------------------------------------------------------------
//HAL_DateToBin
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 HAL_DateToBin(pDateStc_t DatePt, pDateStc_t BinPt)
 * @brief	  Read the value of the date structure, translate them to 48-bit binary
			count and then program the binary count to the RTC counter.
 * @param	  [in] DatePt
 * @param	  [in] BinPt
 * @retval	  return = SUCCESS / FAIL
 * @see 	  HAL_DateToBin
 * @author	  Suny
 * @since	  2010-4-26
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 HAL_GlobalDateToBin(pDateStc_t DatePt, pDateStc_t BinPt) USING_0
{
	UINT8 status;
	UINT16 DMonth[14];
	UINT16 iday;
	UINT32 isec;
	UINT32 tmp0;

	DMonth[0]  = 0;
	DMonth[1]  = 0;
	DMonth[2]  = 31;
	DMonth[3]  = 59;
	DMonth[4]  = 90;
	DMonth[5]  = 120;
	DMonth[6]  = 151;
	DMonth[7]  = 181;
	DMonth[8]  = 212;
	DMonth[9]  = 243;
	DMonth[10] = 273;
	DMonth[11] = 304;
	DMonth[12] = 334;
	DMonth[13] = 365;

	//PRINT_HAL("HAL_DateToBin: Enter\n");

	if ( (DatePt->Second<60) || (DatePt->Minute<60) ||
		(DatePt->Hour<24) || (DatePt->Day<32) ||
		(DatePt->Month<13) || (DatePt->Year<100) ){

		iday = 365*((UINT16)DatePt->Year - 1)+
		DMonth[DatePt->Month]+
		(UINT16)DatePt->Day - 1; //days since 2001/1/1

		iday = iday+((UINT16)DatePt->Year - 1) / 4; //leap days since 2001/1/1

		if((DatePt->Month > 2) && ((DatePt->Year % 4 )==0))
			iday++; //add this year's leap day

		isec = (UINT32)DatePt->Second +
		60*((UINT32)DatePt->Minute +
		60*((UINT32)DatePt->Hour + 24*(UINT32)iday)); //seconds since 2001/1/1

		BinPt->Second = 0;
		if(isec%2)
			BinPt->Minute = 0x80;
		else
			BinPt->Minute = 0x00;

		tmp0 = isec>>1;
		BinPt->Hour  = M_LoByteOfDword(tmp0);
		BinPt->Day	 = M_MidLoByteOfDword(tmp0);
		BinPt->Month = M_MidHiByteOfDword(tmp0);
		BinPt->Year  = M_HiByteOfDword(tmp0);

		status = HALK_SUCCESS;
	}
	else{
		status = HALK_ERROR_PARAMETER;
		//PRINT_HAL("HAL_DateToBin: Error Code = %bx\n", status);
	}
	//PRINT_HAL("HAL_DateToBin: Exit\n");
	return(status);
}

//-----------------------------------------------------------------------------
//HAL_WaitRTCRdy
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 HAL_WaitRTCRdy(void)
 * @brief	  Wait RTC is ready in a period time.
 * @retval	  return = SUCCESS / FAIL
 * @see 	  	  NONE
 * @author	  deleigu
 * @since	  2012-12-11
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 HAL_WaitRTCRdy(void)
{
	UINT8 status;
	UINT32 timeout;

	status = FAIL;
	timeout = 50000;

	do{
		//if((XBYTE[0x206A] & 0x01) == 0x01)
		if(XBYTE[0x206A] != 0x00)
		{
			status = SUCCESS;
			break;
		}
	}while(--timeout); // polling until RTC is ready

	return(status);
}

//-----------------------------------------------------------------------------
//HAL_GlobalRWRTCDataStsGet
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 HAL_GlobalRWRTCDataStsGet(void)
 * @brief	  Get the status of rw rtc data.
 * @retval	  return the status of  rw rtc
 * @see 	  	  NONE
 * @author	  deleigu
 * @since	  2012-12-12
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 HAL_GlobalRWRTCDataStsGet(void)
{
	UINT8 status;

	status = rwRTCDataSts;
	return(status);
}

//-----------------------------------------------------------------------------
//HAL_GlobalWriteRTCData
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 HAL_GlobalWriteRTCData(UINT8 Addr, UINT8 Data)
 * @brief	  Write the value of the date.
 * @param	  [in] Addr: RTC address
 * @param	  [in] Data: RTC data
 * @retval	  return = SUCCESS / FAIL
 * @see 	  HAL_WriteRTCData
 * @author	  Suny
 * @since	  2010-4-26
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 HAL_GlobalWriteRTCData(UINT8 Addr, UINT8 Data)
{
	UINT8 status;

	status = SUCCESS;

	//PRINT_HAL("			 HAL_GlobalWriteRTCData: Enter\n");

	XBYTE[0x2067] = 0x01; //enable RTC serial port
	XBYTE[0x206B] = Addr; //RTC address
	XBYTE[0x206C] = Data; //RTC write data
	XBYTE[0x2069] = 0x01; //write RTC register
	//while(XBYTE[0x206A]==0x00); // polling until RTC is ready
	status = HAL_WaitRTCRdy();

	XBYTE[0x2067] = 0x00; //disable RTC serial port

	//PRINT_HAL("			 HAL_GlobalWriteRTCData: Exit\n");
	rwRTCDataSts |= status;
	return(status);
}
/**
 * @fn		  UINT8 HAL_GlobalWriteRTCData(UINT8 Addr, UINT8 Data)
 * @brief	  Write the value of the date.
 * @param	  [in] Addr: RTC address
 * @param	  [in] Data: RTC data
 * @retval	  return = SUCCESS / FAIL
 * @see 	  HAL_WriteRTCData
 * @author	  Suny
 * @since	  2010-4-26
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 HAL_GlobalWriteRTCDataIntr(UINT8 Addr, UINT8 Data)
{
	UINT8 status;
	UINT32 timeout;

	status = SUCCESS;

	//PRINT_HAL("			 HAL_GlobalWriteRTCData: Enter\n");

	XBYTE[0x2067] = 0x01; //enable RTC serial port
	XBYTE[0x206B] = Addr; //RTC address
	XBYTE[0x206C] = Data; //RTC write data
	XBYTE[0x2069] = 0x01; //write RTC register
	//while(XBYTE[0x206A]==0x00); // polling until RTC is ready
	timeout = 50000;
	do{
		if(XBYTE[0x206A] != 0x00)
		{
			status = SUCCESS;
			break;
		}
	}while(--timeout);

	XBYTE[0x2067] = 0x00; //disable RTC serial port

	//PRINT_HAL("			 HAL_GlobalWriteRTCData: Exit\n");
	rwRTCDataSts |= status;
	return(status);
}

//-----------------------------------------------------------------------------
//HAL_GlobalWriteRTC
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 HAL_GlobalWriteRTC(pDateStc_t DatePt)
 * @brief	  Write the value of the date structure, translate them to 48-bit binary
			count and then program the binary count to the RTC counter.
 * @param	  [in] * DatePt : the value of the date structure that is composed of the data defined as follows:
				UINT8 Year,
				UINT8 Month,
				UINT8 Day,
				UINT8 Hour,
				UINT8 Minute,
				UINT8 Second.
 * @retval	  return = SUCCESS / FAIL
 * @see 	  HAL_GlobalWriteRTC
 * @author	  Suny
 * @since	  2010-4-26
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 HAL_GlobalWriteRTC(pDateStc_t DatePt) USING_0
{
	dateStc_t Bin;
	UINT8 status0, status1, status2, status3, status4, status5, status6, status7;
	UINT8 status,alrmint;

	//body
	//PRINT_HAL("		 HAL_WriteRTC: Enter\n");		
	HAL_GlobalWriteRTCData(0x02, 0x5a); //Secint/alarmint enable
	HAL_GlobalReadRTCData(0xd0, &alrmint); //read alrmint enable sts
	if ((alrmint & 0x02) == 0x02 ) {
		HAL_GlobalWriteRTCData(0xd0, 0x00); //Secint/alarmint disable
	}
	HAL_GlobalWriteRTCData(0xc0, 0x00); //Clear Secint/alarmint
	HAL_GlobalWriteRTCData(0x00, 0x0f); //RTC reset
	HAL_GlobalWriteRTCData(0x03, 0x01); //FDEn
	HAL_GlobalWriteRTCData(0x00, 0x0d); //loadcnt/up-count, release the reset
	HAL_GlobalWriteRTCData(0xc0, 0x00); //Clear Secint/alarmint
	if ((alrmint & 0x02) == 0x02 ){
		HAL_GlobalWriteRTCData(0xd0, 0x02); //Secint/alarmint enable
	}
	status0 = HAL_GlobalDateToBin(DatePt, &Bin); //translate the date to binary count

	status1 = HAL_GlobalWriteRTCData(0x10, Bin.Second);
	status2 = HAL_GlobalWriteRTCData(0x11, Bin.Minute);
	status3 = HAL_GlobalWriteRTCData(0x12, Bin.Hour);
	status4 = HAL_GlobalWriteRTCData(0x13, Bin.Day);
	status5 = HAL_GlobalWriteRTCData(0x14, Bin.Month);
	status6 = HAL_GlobalWriteRTCData(0x15, Bin.Year);

	status7 = HAL_GlobalWriteRTCData(0xB0, 0x01); //load timer

	status = status0|status1|status2|status3|status4|status5|status6|status7;

	//PRINT_HAL("		 HAL_WriteRTC: Exit\n");

	return(status);
}

//-----------------------------------------------------------------------------
//HAL_GlobalReadRTCData
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 HAL_GlobalReadRTCData(UINT8 Addr, UINT8* DataPt)
 * @brief	  Read the value of RTC counter and translate them to the value of Year,
			Month, Date, Hour, Minute and Second.
 * @param	  [in] Addr
 * @param	  [in] * DatePt: the value of the date structure
 * @retval	  return = SUCCESS / FAIL
 * @see 	  HAL_ReadRTCData
 * @author	  Suny
 * @since	  2010-4-26
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 HAL_GlobalReadRTCData(UINT8 Addr, UINT8* DataPt)
{
	UINT8 status;

	status = HALK_SUCCESS;

	//PRINT_HAL("HAL_GlobalReadRTCData: Enter\n");

	XBYTE[0x2067] = 0x01; //enable RTC serial port
	XBYTE[0x206B] = Addr; //RTC address
	XBYTE[0x2069] = 0x02; //read RTC register
	//while(XBYTE[0x206A]==0x00); // polling until RTC is ready
	status = HAL_WaitRTCRdy();
	
	*DataPt = XBYTE[0x206D];

	XBYTE[0x2067] = 0x00; //disable RTC serial port

	//PRINT_HAL("HAL_GlobalReadRTCData: Exit\n");
	rwRTCDataSts |= status;
	return(status);
}

//-----------------------------------------------------------------------------
//HAL_BinToDate
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 HAL_BinToDate(pDateStc_t BinPt, pDateStc_t DatePt)
 * @brief	  convert binary to date
 * @param	  [in] pDateStc_t BinPt: the value of the date structure
 * @param	  [in] pDateStc_t DatePt: the value of the date structure
 * @retval	  return = SUCCESS / FAIL
 * @see 	  HAL_BinToDate
 * @author	  Suny
 * @since	  2010-4-26
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 HAL_GlobalBinToDate(pDateStc_t BinPt, pDateStc_t DatePt)
{
	UINT8 status;
	UINT8 iyrs, mon;
	UINT16 DMonth[14];
	UINT16 iday, lday, qday, jday, mday;
	UINT32 imin, ihrs;
	UINT32 tmp0;

	status = HALK_SUCCESS;

	DMonth[0]  = 0;
	DMonth[1]  = 0;
	DMonth[2]  = 31;
	DMonth[3]  = 59;
	DMonth[4]  = 90;
	DMonth[5]  = 120;
	DMonth[6]  = 151;
	DMonth[7]  = 181;
	DMonth[8]  = 212;
	DMonth[9]  = 243;
	DMonth[10] = 273;
	DMonth[11] = 304;
	DMonth[12] = 334;
	DMonth[13] = 365;

	//PRINT_HAL("			 HAL_BinToDate: Enter\n");

	//whole seconds since 2001/1/1
	tmp0 = BinPt->Year;
	tmp0 = (tmp0<<8)|(UINT32)BinPt->Month;
	tmp0 = (tmp0<<8)|(UINT32)BinPt->Day;
	tmp0 = (tmp0<<8)|(UINT32)BinPt->Hour;
	tmp0 = tmp0<<1;

	if(BinPt->Minute & 0x80)
		tmp0 = tmp0|0x00000001;

	imin = tmp0/60; //whole minutes since 2001/1/1
	DatePt->Second = tmp0-(60*imin);

	ihrs = imin/60; //whole hours since 2001/1/1
	DatePt->Minute = imin-60*ihrs;

	iday = ihrs/24;
	DatePt->Hour = ihrs-24*iday;

	iday = iday+366; //whole days sicne 2000/1/1
	lday = iday/1461; //quadyr = 4 yr period = 1461 days
	qday = iday%1461; //days since current quadyr begin

	if(qday>=60)
		lday++; //if past 2/29 then add this quadyr's leap day

	iyrs = (iday-lday)/365; //whole years since 2001/1/1
	jday = iday-(iyrs*365)-lday; //days since 1/1 of current year

	if((qday<=365) && (qday>=60))
		jday++; //if past 2/29 and a leap year then add a leap day

	DatePt->Year = iyrs;

	mon = 13;
	mday = 366;

	while(jday<mday)
	{
		mon--;
		mday = DMonth[mon];

		if((mon>2) && ((iyrs%4)==0))
			mday++; //if past 2/29 and leap year then add leap day
	}

	DatePt->Month = mon;
	DatePt->Day = jday-mday+1;

	//PRINT_HAL("			 HAL_BinToDate: Exit\n");

	if(DatePt->Year>30)
	{
		DatePt->Year = 10;
		DatePt->Month =1;
		DatePt->Day = 1;
		DatePt->Hour = 12;
		DatePt->Minute = 0;
		DatePt->Second = 0;

		HAL_GlobalWriteRTC(DatePt);
	}	

	return(status);
}

//-----------------------------------------------------------------------------
//HAL_GlobalReadRTC
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 HAL_GlobalReadRTC(pDateStc_t DatePt)
 * @brief	  Read the value os the date structre
 * @param	  [in] pDateStc_t DatePt: the value of the date structure
 * @retval	  return = SUCCESS / FAIL
 * @see 	  HAL_ReadRTC
 * @author	  Suny
 * @since	  2010-4-26
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 HAL_GlobalReadRTC(pDateStc_t DatePt)
{
	dateStc_t Bin;
	UINT8 first0, first1, first2, first3, first4, first5;
	UINT16 first10;
	UINT8 status;
	UINT8 snap_sts; 						// Jay add
	UINT16 timeout;

	status = HALK_SUCCESS;
	timeout = 500;

	//PRINT_HAL("		 HAL_GlobalReadRTC: Enter\n");

	HAL_GlobalWriteRTCData(0x00, 0x05);	// snap timer mux, YP added
	HAL_GlobalWriteRTCData(0xb1, 0x01);   // timer snap, YP added

	/* wait for snap status ready */
	snap_sts = 0;							// Jay add
	while((snap_sts&0x10)==0x00 && (--timeout))			// Jay add
		HAL_GlobalReadRTCData(0x00, &snap_sts);	// Jay add
	if(timeout == 0)
		status = HALK_ERROR_GENERAL;

	status |= HAL_GlobalReadRTCData(0x10, &first0);	// YP modified
	status |= HAL_GlobalReadRTCData(0x11, &first1);	// YP modified
	status |= HAL_GlobalReadRTCData(0x12, &first2);	// YP modified
	status |= HAL_GlobalReadRTCData(0x13, &first3);	// YP modified
	status |= HAL_GlobalReadRTCData(0x14, &first4);	// YP modified
	status |= HAL_GlobalReadRTCData(0x15, &first5);	// YP modified
	first10 = ((UINT16)first1<<8)|(UINT16)first0;

	Bin.Second = first0;
	Bin.Minute = first1;
	Bin.Hour   = first2;
	Bin.Day    = first3;
	Bin.Month  = first4;
	Bin.Year   = first5;

	HAL_GlobalBinToDate(&Bin, DatePt); //translate binary count to date
	#if 0
	PRINT_GLOBAL("HAL_GlobalReadRTC: Second = %bu\n", DatePt->Second);
	PRINT_GLOBAL("HAL_GlobalReadRTC: Minute = %bu\n", DatePt->Minute);
	PRINT_GLOBAL("HAL_GlobalReadRTC: Hour	= %bu\n", DatePt->Hour);
	PRINT_GLOBAL("HAL_GlobalReadRTC: Day	= %bu\n", DatePt->Day);
	PRINT_GLOBAL("HAL_GlobalReadRTC: Month	= %bu\n", DatePt->Month);
	PRINT_GLOBAL("HAL_GlobalReadRTC: Year	= %bu\n", DatePt->Year);
	#endif
	//PRINT_HAL("		 HAL_GlobalReadRTC: Exit\n");

	return(status);
}

//-----------------------------------------------------------------------------
//HAL_WriteAlarm
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 HAL_WriteAlarm(pDateStc_t DatePt)
 * @brief	  Write the value of the date structure, translate them to 48-bit binary
			count and then program the binary count to the RTC counter.
 * @param	  [in] *DatePt : the value of the date structure that is composed of the data defined as follows:
				  UINT8 Year,
				  UINT8 Month,
				  UINT8 Day,
				  UINT8 Hour,
				  UINT8 Minute,
				  UINT8 Second.
 * @retval	  return = SUCCESS / FAIL
 * @see 	  HAL_WriteAlarm
 * @author	  Suny
 * @since	  2010-4-26
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 HAL_GlobalWriteAlarm(pDateStc_t DatePt) USING_0
{
	dateStc_t Bin;
	UINT8 status0, status1, status2, status3, status4, status5, status6;
	UINT8 status;

	//PRINT_HAL("		 HAL_WriteAlarm: Enter\n");

	status0 = HAL_GlobalDateToBin(DatePt, &Bin); //translate the date to binary count

	status1 = HAL_GlobalWriteRTCData(0x20, Bin.Second);
	status2 = HAL_GlobalWriteRTCData(0x21, Bin.Minute);
	status3 = HAL_GlobalWriteRTCData(0x22, Bin.Hour);
	status4 = HAL_GlobalWriteRTCData(0x23, Bin.Day);
	status5 = HAL_GlobalWriteRTCData(0x24, Bin.Month);
	status6 = HAL_GlobalWriteRTCData(0x25, Bin.Year);

	status = status0|status1|status2|status3|status4|status5|status6;

	//PRINT_HAL("		 HAL_WriteAlarm: Exit\n");

	return(status);
}

//-----------------------------------------------------------------------------
//HAL_ReadAlarm
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 HAL_ReadAlarm(pDateStc_t DatePt)
 * @brief	  Read the value of alarm register and translate them to the value of Year,
			Month, Date, Hour, Minute and Second.
 * @param	  [in] pDateStc_t DatePt: the value of the date structure
 * @retval	  return = SUCCESS / FAIL
 * @see 	  HAL_ReadAlarm
 * @author	  Suny
 * @since	  2010-4-26
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 HAL_GlobalReadAlarm(pDateStc_t DatePt) USING_0
{
	dateStc_t Bin;
	UINT8 status0, status1, status2, status3, status4, status5, status6;
	UINT8 status;

	//body
	//PRINT_HAL("		 HAL_ReadAlarm: Enter\n");

	status0 = HAL_GlobalReadRTCData(0x20, &(Bin.Second));
	status1 = HAL_GlobalReadRTCData(0x21, &(Bin.Minute));
	status2 = HAL_GlobalReadRTCData(0x22, &(Bin.Hour));
	status3 = HAL_GlobalReadRTCData(0x23, &(Bin.Day));
	status4 = HAL_GlobalReadRTCData(0x24, &(Bin.Month));
	status5 = HAL_GlobalReadRTCData(0x25, &(Bin.Year));

	status6 = HAL_GlobalBinToDate(&Bin, DatePt); //translate binary count to date

	status = status0|status1|status2|status3|status4|status5|status6;
	#if 0
	PRINT_HAL(" 	   HAL_ReadAlarm: Second = %bu\n", DatePt->Second);
	PRINT_HAL(" 	   HAL_ReadAlarm: Minute = %bu\n", DatePt->Minute);
	PRINT_HAL(" 	   HAL_ReadAlarm: Hour	 = %bu\n", DatePt->Hour);
	PRINT_HAL(" 	   HAL_ReadAlarm: Day	 = %bu\n", DatePt->Day);
	PRINT_HAL(" 	   HAL_ReadAlarm: Month  = %bu\n", DatePt->Month);
	PRINT_HAL(" 	   HAL_ReadAlarm: Year	 = %bu\n", DatePt->Year);
	#endif

	//PRINT_HAL("		 HAL_ReadAlarm: Exit\n");

	return(status);
}

/**
 * @fn		  UINT8 Hal_GlobalAlarmRTCIntrSet(UINT8 enable)
 * @brief	  Hal_GlobalAlarmRTCIntrSet
 * @param	  [in] enable
 * @retval	 Alarm IntrEvent
 * @see 	  sp1kHalAlarmRTCIntrSet
 * @author	  Phil Lin
 * @since	  2010-4-27
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 Hal_GlobalAlarmRTCIntrSet(UINT8 enable)
{
	UINT8 value = 0;
	HAL_GlobalReadRTCData(0xc0, &value); /*read flag*/
	HAL_GlobalWriteRTCData(0xc0, 0); /*clear alarm*/
	if(enable){
		HAL_GlobalWriteRTCData(0xd0, 0x02); /*enable alarm*/
		//XBYTE[0x20EB] |= 0x01;//Enable RTC interrupt
		XBYTE[0x20D0] |= 0x20;//Enable RTC interrupt
	}
	else{
		HAL_GlobalWriteRTCData(0xd0, 0); /*disable alarm*/
		//XBYTE[0x20EB] &= 0xfe;//Disable RTC interrupt
		XBYTE[0x20D0] &= (~0x20);//Disable RTC interrupt
	}

	return value;
}
// 2007.12.05@ suny mark end


//-----------------------------------------------------------------------------
//HAL_ConfigPG
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 HAL_ConfigPG(UINT8 mode, UINT8 polarity, polarity,UINT8 idlstate, idlstate,UINT16 lowDuty, lowDuty,UINT16 highDuty, highDuty,UINT8 state)
 * @brief	  Config PWM out waveform when need PWM signal.Set the PWM waveform parameter
 * @param	  [in] mode:
				0 slecet PWM0 output
				1 slecet PWM1 output
				2 slecet PWM2 output
				3 slecet PWM3 output
 * @param	  [in] polarity:
				0 normal waveform
				1 reverse waveform
 * @param	  [in] idlstate:
				0 signal stays at 0 in idle state
				1 signal stays at 1 in idle state
 * @param	  [in] lowDuty: Low period
 * @param	  [in] highDuty: High period
 * @param	  [in] state:
				0 PWM in idle state
				1 PWM in opration
 * @retval	  return = SUCCESS / FAIL
 * @see 	  HAL_ConfigPG
 * @author	  kai.wang
 * @since	  2010-4-26
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 HAL_GlobalConfigPG(UINT8 mode, UINT8 polarity,UINT8 idlstate,UINT16 lowDuty,UINT16 highDuty,UINT8 state) USING_0
{
	UINT8 status;

	status = HALK_SUCCESS;

	if ( mode == 0 ) {
		//HAL_GpioByteFuncSet(HAL_GPIO_BYTE_GEN0, 0x10, 0x00);  // set gpio[4] to func.
		XBYTE[0x201b] |= 0x01;
		if ( polarity == 0 ) {
			XBYTE[0x2091] &=0xFE;	//normal mode
		} else if ( polarity == 1 ) {
			XBYTE[0x2091] |=0x01;	//reverse the PWM0 waveform
		}

		if ( idlstate == 0 ) {
			XBYTE[0x2091] &= 0xFD;	//PWM0 signal stays at 0 in idle state
		} else if ( idlstate == 1 ) {
			XBYTE[0x2091] |= 0x02;	//PWM0 signal stays at 1 in idle state
		}

		XBYTE[0x2093] = (UINT8)lowDuty;
		XBYTE[0x2094] = (UINT8)((lowDuty >> 8)&0x3F);
		XBYTE[0x2095] = (UINT8)highDuty;
		XBYTE[0x2096] = (UINT8)((highDuty >> 8)&0x3F);

		if( state == 1 ) {
			XBYTE[0x2092] &= 0xFE;		//PWM0 in opration
		} else if ( state == 0 ) {
			XBYTE[0x2092] |= 0x01;		//PWM0 in idle state
		}
	}
	else if ( mode == 1 ) {
		//HAL_GpioByteFuncSet(HAL_GPIO_BYTE_GEN0, 0x02, 0x00);  // set gpio[5] to func.
		XBYTE[0x201b] |= 0x02;
		XBYTE[0x2034] |= 0x20;
		if ( polarity == 0 ) {
			XBYTE[0x2099] &=0xFE;	//normal mode
		} else if ( polarity == 1 ){
			XBYTE[0x2099] |=0x01;	//reverse the PWM1 waveform
		}

		if( idlstate == 0 ) {
			XBYTE[0x2099] &= 0xFD;	//PWM1 signal stays at 0 in idle state
		} else if ( idlstate == 1 ) {
			XBYTE[0x2099] |= 0x02;	//PWM1 signal stays at 1 in idle state
		}

		XBYTE[0x209B] = (UINT8)lowDuty;
		XBYTE[0x209C] = (UINT8)((lowDuty >> 8)&0x07);
		XBYTE[0x209D] = (UINT8)highDuty;
		XBYTE[0x209E] = (UINT8)((highDuty >> 8)&0x07);

		if( state == 1 ) {
			XBYTE[0x209A] &= 0xFE;		//PWM1 in opration
		} else if( state == 0 ) {
			XBYTE[0x209A] |= 0x01;		//PWM1 in idle state
		}
	}
	else if ( mode == 2 ) {
		//HAL_GpioByteFuncSet(HAL_GPIO_BYTE_GEN0, 0x04, 0x00);  // set gpio[6] to func.
		XBYTE[0x201b] |= 0x04;
		XBYTE[0x2034] |= 0x40;
		if ( polarity == 0 ) {
			XBYTE[0x20B1] &=0xFE;	//normal mode
		} else if (polarity ==1){
			XBYTE[0x20B1] |=0x01;	//reverse the PWM1 waveform
		}

		if(idlstate == 0) {
			XBYTE[0x20B1] &= 0xFD;	//PWM2 signal stays at 0 in idle state
		} else if ( idlstate == 1 ) {
			XBYTE[0x20B1] |= 0x02;	//PWM2 signal stays at 1 in idle state
		}

		XBYTE[0x20B3] = (UINT8)lowDuty;
		XBYTE[0x20B4] = (UINT8)((lowDuty >> 8)&0x07);
		XBYTE[0x20B5] = (UINT8)highDuty;
		XBYTE[0x20B6] = (UINT8)((highDuty >> 8)&0x07);

		if( state == 1 ) {
			XBYTE[0x20B2] &= 0xFE;		//PWM2 in opration
		} else if( state == 0 ) {
			XBYTE[0x20B2] |= 0x01;		//PWM2 in idle state
		}
	}
	else if ( mode == 3 ) {
		//HAL_GpioByteFuncSet(HAL_GPIO_BYTE_GEN1, 0x40, 0x00);  // set gpio[14] to func.
		XBYTE[0x201b] |= 0x08;
		XBYTE[0x2035] |= 0x40;
		if ( polarity == 0 ) {
			XBYTE[0x20B9] &=0xFE;	//normal mode
		} else if ( polarity == 1 ){
			XBYTE[0x20B9] |=0x01;	//reverse the PWM1 waveform
		}

		if( idlstate == 0 ) {
			XBYTE[0x20B9] &= 0xFD;	//PWM3 signal stays at 0 in idle state
		} else if ( idlstate == 1 ) {
			XBYTE[0x20B9] |= 0x02;	//PWM3 signal stays at 1 in idle state
		}

		XBYTE[0x20BB] = (UINT8)lowDuty;
		XBYTE[0x20BC] = (UINT8)((lowDuty >> 8)&0x07);
		XBYTE[0x20BD] = (UINT8)highDuty;
		XBYTE[0x20BE] = (UINT8)((highDuty >> 8)&0x07);

		if( state == 1 ) {
			XBYTE[0x20BA] &= 0xFE;		//PWM3 in opration
		} else if( state == 0 ) {
			XBYTE[0x20BA] |= 0x01;		//PWM3 in idle state
		}
	}
	return(status);
}

//-----------------------------------------------------------------------------
//HAL_GlobalStartPG
//-----------------------------------------------------------------------------
/**
 * @fn		  void HAL_GlobalStartPG(UINT8 mode)
 * @brief	  start PWM waveform output when need PWM signal
 * @param	  [in] mode:
				0 slecet PWM0 output, 
				1 slecet PWM1 output
 * @retval	  NONE
 * @see 	  HAL_StartPG
 * @author	  kai.wang
 * @since	  2010-4-26
 * @todo	  N/A
 * @bug 	  N/A
*/
void HAL_GlobalStartPG(UINT8  mode) USING_0
{
	if((mode != 0)&&(mode != 1)&&(mode != 2)&&(mode != 3))
		return;

	if (mode == 0) {
		XBYTE[0x2090] = 0x01;		//start PWM0
	}
	else if (mode == 1) {
		XBYTE[0x2098] = 0x01;		//start PWM1
	}
	else if (mode == 2) {
		XBYTE[0x20B0] = 0x01;		//start PWM2
	}
	else {
		XBYTE[0x20B8] = 0x01;		//start PWM3
	}
}

//-----------------------------------------------------------------------------
//HAL_GlobalStopPG
//-----------------------------------------------------------------------------
/**
 * @fn		  void HAL_GlobalStopPG(UINT8 mode)
 * @brief	  Stop PWM waveform output when need  not PWM signal
 * @param	  [in] mode:
				0 stop PWM0 output
				1 stop PWM1 output
 * @retval	  NONE
 * @see 	  HAL_StopPG
 * @author	  Phil Lin
 * @since	  2010-4-26
 * @todo	  N/A
 * @bug 	  N/A
*/
void HAL_GlobalStopPG(UINT8  mode) USING_0
{
	if((mode != 0)&&(mode != 1)&&(mode != 2)&&(mode != 3))
		return;

	if (mode == 0) {
		XBYTE[0x2090] = 0x02;	//stop PWM0
	}
	else if (mode == 1) {
		XBYTE[0x2098] = 0x02;	//stop PWM1
	}
	else if (mode == 2) {
		XBYTE[0x20B0] = 0x02;	//stop PWM2
	}
	else {
		XBYTE[0x20B8] = 0x02;	//stop PWM3
	}
}



/**
 * @fn		  UINT8 HAL_GlobalReadRegister(UINT16 reg)
 * @brief	  HAL_GlobalReadRegister
 * @param	  [in] reg
 * @retval	  return = SUCCESS / FAIL
 * @see 	  HAL_ReadRegister
 * @author	  Phil Lin
 * @since	  2010-4-27
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 HAL_GlobalReadRegister(UINT16 reg)
{

	return XBYTE[reg];
}

/**
 * @fn		  void HAL_GlobalWriteRegister(UINT16 reg, UINT8 value)
 * @brief	  HAL_GlobalWriteRegister
 * @param	  [in] reg
 * @param	  [in] value
 * @retval	  NONE
 * @see 	  HAL_WriteRegister
 * @author	  Phil Lin
 * @since	  2010-4-27
 * @todo	  N/A
 * @bug 	  N/A
*/
void HAL_GlobalWriteRegister(UINT16 reg, UINT8 value)
{
	XBYTE[reg] = value;

	return;
}

//-----------------------------------------------------------------------------
//HAL_GlobalCamModeSet
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 HAL_GlobalCamModeSet(sp1kCamMode_t CamMode)
 * @brief	  Set camera operation mode, the related initialization must be done before
			calling this function.
 * @param	  [in] CamMode: the camera operation mode
			0: idle mode
			1: preview mode
			2: still camera mode
			3: video clip mode
			4: PC-camera mode
			5: playback mode
			6: Upload/Download mode
 * @retval	  return = SUCCESS / FAIL
 * @see 	  HAL_GlobalCamModeSet
 * @author	  suny
 * @since	  2010-4-26
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 HAL_GlobalCamModeSet(sp1kCamMode_t CamMode) USING_0
{
	UINT8 status;

	status = HALK_SUCCESS;

	XBYTE[0x2000] = CamMode;

	return(status);
}


//-----------------------------------------------------------------------------
//HAL_GlobalCamModeGet
//-----------------------------------------------------------------------------
/**
 * @fn		  void HAL_GlobalCamModeGet(UINT8* CamModePt)
 * @brief	  Get camera operation mode.
 * @param	  [in] *CamModePt: The camera operation mode
 * @retval	  NONE
 * @see 	  HAL_GetCamMode
 * @author	  suny
 * @since	  2010-4-26
 * @todo	  N/A
 * @bug 	  N/A
*/
void HAL_GlobalCamModeGet(UINT8* CamModePt) USING_0
{
  *CamModePt = XBYTE[0x2000];
  
  return;
}

//-----------------------------------------------------------------------------
//HAL_ReadIOTrap
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT16 HAL_GlobalReadIOTrap(void)
 * @brief	  Read the IO trap value.
 * @param	  NONE
 * @retval	  :the IO trap map
 * @see 	  HAL_ReadIOTrap
 * @author	  kai.wang
 * @since	  2010-4-26
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT16	HAL_GlobalReadIOTrap(void) USING_0
{
	UINT8 temp;
	UINT16 retval;	
  
	temp = XBYTE[0x20FA];
	retval = (((UINT16)temp<<8) | XBYTE[0x20F9]) | 0xfc00;
	return retval;
}

//-----------------------------------------------------------------------------
//HAL_GlobalRTCTransferText
//-----------------------------------------------------------------------------
/**
 * @fn		  void HAL_GlobalRTCTransferText(pDateStc_t DateTime, DateTime,UINT8* TimeString)
 * @brief	  Convert date to string.
 * @param	  [in] DateTime
 * @param	  [in] TimeString
 * @retval	  NONE
 * @see 	  HAL_ReadIOTrap
 * @author	  suny
 * @since	  2010-4-26
 * @todo	  N/A
 * @bug 	  N/A
*/
void HAL_GlobalRTCTransferText(pDateStc_t DateTime,UINT8* TimeString) USING_0
{
	 //year
	 *TimeString = '2';
	 *(TimeString + 1) = '0';
	 *(TimeString + 2) = (DateTime->Year / 10) + '0';
	 *(TimeString + 3) = (DateTime->Year % 10) + '0';

	*(TimeString +4) = ':';
	 //month
	 *(TimeString + 5) = (DateTime->Month / 10) + '0';
	 *(TimeString + 6) = (DateTime->Month % 10) + '0';

	*(TimeString +7) = ':';
	 //day
	 *(TimeString + 8) = (DateTime->Day / 10) + '0';
	 *(TimeString + 9) = (DateTime->Day % 10) + '0';

	*(TimeString +10) = ' ';
	 //hour
	 *(TimeString + 11) = (DateTime->Hour / 10) + '0';
	 *(TimeString + 12) = (DateTime->Hour % 10) + '0';

	*(TimeString +13) = ':';
	 //minute
	 *(TimeString + 14) = (DateTime->Minute / 10) + '0';
	 *(TimeString + 15) = (DateTime->Minute % 10) + '0';

	*(TimeString +16) = ':';
	 //second
	 *(TimeString + 17) = (DateTime->Second / 10) + '0';
	 *(TimeString + 18) = (DateTime->Second % 10) + '0';
}

/**
 * @fn		  void HAL_GlobalSuspendResume(void)
 * @brief	  Do Suspend Resume.
 * @param	  NONE
 * @retval	  NONE
 * @see 	  HAL_SuspendResume
 * @author	  Eric Su
 * @since	  2010-4-26
 * @todo	  N/A
 * @bug 	  N/A
*/
void HAL_GlobalSuspendResume(void) USING_0
{
	//UINT8 status;
	UINT8 reg20a2,reg20a3;//IO trap 
	XBYTE[REG_Cpu_ShadowImemL] = 0x04;	/* ROM space start address 0x39000 mapping to imem */
	XBYTE[REG_Cpu_ShadowImemH] = 0x00;
	XBYTE[REG_Cpu_ShadowEn] |= 0x01;	/* Shadow imem enable */
	//Supend
	XBYTE[0x20e3] = 0x03;//adjust 96m clock phase (B version will be fixed)

	XBYTE[0x2004] = 0x06;//Resume 15 ms
	XBYTE[REG_Dram_EnterSrf] |= 0x01;//ENTER SDRAM self-refresh
	
	//XBYTE[0x200d] = 0x01;//SDRAM auto exit self-refresh
	//ENTER_CRITICAL( status );
	reg20a2 = XBYTE[0x20a2];//save IO trap sts 
	reg20a3 = XBYTE[0x20a3];
	
	XBYTE[0x20a2] |= ((~(XBYTE[0x20F9] <<4)) & 0xf0);
	XBYTE[0x20a3] |= ((~((XBYTE[0x20F9] >>4)|(XBYTE[0x20fa]<<4))) & 0x1f);

	
	XBYTE[0x200c] = 0x01;//enable suspend
	//Resume
	XBYTE[0x20a2] = reg20a2;
	XBYTE[0x20a3] = reg20a3;
	
	XBYTE[REG_Dram_EnterSrf] &= ~0x01;//EXIT SDRAM self-refresh
	//EXIT_CRITICAL( status );

	XBYTE[0x200c] = 0x00;//disable suspend
	//Resume
}


/**
 * @fn		  void HAL_GlobalPwrCtrl(UINT8 cpuclk, UINT8 dramclk, UINT8 reg2013, UINT8 reg2014, UINT8 reg2015)
 * @brief	  Power control hal function
 * @param	  [in] cpuclk
 * @param	  [in] dramclk
 * @param	  [in] reg2013
 * @param	  [in] reg2014
 * @param	  [in] reg2015
 * @retval	  NONE
 * @see 	  HAL_PwrCtrl
 * @author	  Phil Lin
 * @since	  2010-4-26
 * @todo	  N/A
 * @bug 	  N/A
*/
void HAL_GlobalPwrCtrl(UINT8 cpuclk, UINT8 dramclk, UINT8 reg2013, UINT8 reg2014, UINT8 reg2015) USING_0
{
//	printf("c,d=%bx,%bx\n",cpuclk,dramclk);
	
	cpuclk = cpuclk;
	dramclk = dramclk;

	XBYTE[0x2013] = reg2013;
	XBYTE[0x2014] = reg2014;
	XBYTE[0x2015] = reg2015;
}

/**
 * @fn		  void HAL_GlobalDramClkSet(UINT8 DramClk)
 * @brief	  DRAM clock switching.
 * @param	  [in] DramClk
 * @retval	  NONE
 * @see 	  HAL_DramClkSet
 * @author	  Allen Lin
 * @since	  2010-4-26
 * @todo	  N/A
 * @bug 	  N/A
*/
void HAL_GlobalDramClkSet(UINT8 DramClk) USING_0
{
	if (DramClk == 48){
		/* 48M */
		XBYTE[0x2025] = 0x01;	//depend on 96M/48M, no 32M/24M allowable
	}
	else {
		/* 96M */
		XBYTE[0x2025] = 0x00;	//depend on 96M/48M, no 32M/24M allowable
	}
	XBYTE[REG_Dram_SwFreqInit] = 0x04;	//enable wait dram idle (you can try disable this to see if system break down)
	XBYTE[0x200E] = 0x01;	//trigger update freq
}

/**
 * @fn		  void HAL_GlobalBaudRateSet(UINT32 BaudRate)
 * @brief	  Baud rate setting
 * @param	  [in] BaudRate
 * @retval	  NONE
 * @see 	  HAL_BaudRateSet
 * @author	  Allen Lin
 * @since	  2010-4-26
 * @todo	  N/A
 * @bug 	  N/A
*/
void HAL_GlobalBaudRateSet(UINT32 BaudRate) USING_0
{
	UINT32 CpuClk;
	UINT8 temp;
	
	switch ( XBYTE[0x2024] ) {
		case 0x00:
			CpuClk = 96000000UL;
			break;
		case 0x01:
			CpuClk = 48000000UL;
			break;
		case 0x03:
			CpuClk = 24000000UL;
			break;
		default:
			break;
	}

	temp = 256UL -	CpuClk / (32UL * BaudRate);
	RCAP2L = temp;
}

/**
 * @fn        void HAL_GlobalTimer2Handle_intr(void)
 * @brief     for global timer2 interrupt service function
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
void HAL_GlobalTimer2Handle_intr(void) USING_0
{
	UINT8 flag = 0;
	pFuncGT2* ptr;
	
	//HAL_GlobalTimer2Stop();
	XBYTE[0x20c0] &= 0xF7; /*clear global timer2 interrupt event*/
	XBYTE[0x20cc] = 0x02;
	
	// add by phil for correcting the call tree for function pointer S	
	if (flag) {
		ptr = GTmr2FuncCallList;
	}
	// add by phil for correcting the call tree for function pointer E	
	
	if (GT2PFunc != NULL) {
		GT2PFunc(GT2PFuncPara);
	}
}


/**
 * @fn        UINT8 HAL_GlobalTimer2Config(UINT32 loadtime, UINT8 countmode, UINT8 timebase, cGT2 pFunc, UINT8* pFuncPara)
 * @brief     configure global timer2 which customer could configur this timer according to their needs
 * @param     [in] loadtime :
 * @param     [in] countmode: 0:downcount mode
 *							  1:upcount mode
 * @param     [in] timebase: 0:10us
 *							 1:1ms
 * @param     [in] pFunc: function pointer
 * @param     [in] pFuncPara
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-11
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
HAL_GlobalTimer2Config(
	UINT32 loadtime, 
	UINT8 countmode, 
	UINT8 timebase, 
	pFuncGT2 pFunc, 
	UINT8* pFuncPara
)
{
	UINT8 tempmode ;
	
	HAL_GlobalTimer2Stop();
	
	if ((!loadtime) || (countmode > 1) || (timebase > 1)) {
		return FAIL;
	}

	GT2PFunc = pFunc;
	GT2PFuncPara = pFuncPara;
	
	tempmode = (countmode ) | ((timebase )<<1);	
	
	if ( tempmode == 0x00 ) { /*down count mode and time stick 10us*/
		XBYTE[0x20CB] = 0x00;
	}	
	else if( tempmode == 0x01 ) { /*up count mode and time stick 10us*/
		XBYTE[0x20CB] = 0x01;
	}
	else if( tempmode == 0x02 ) { /*down count mode and time stick 1ms*/
		XBYTE[0x20CB] = 0x02;
	}
	else if( tempmode == 0x03 ) { /*up count mode and time stick 1ms*/
		XBYTE[0x20CB] = 0x03;
	}

	XBYTE[0x20C7] = READ8(loadtime,3);	/* set initial value */
	XBYTE[0x20C8] = READ8(loadtime,2);
	XBYTE[0x20C9] = READ8(loadtime,1);
	XBYTE[0x20CA] = READ8(loadtime,0);
	XBYTE[0x20CC] |= 0x04;	/*load time*/

	//if (pFunc != (void (*)())NULL) {
		XBYTE[0x20D0] |= 0x08;	/* Global timer2 interrupt enable */
	//}
	return SUCCESS;
}

/**
 * @fn        void HAL_GlobalTimer2Start(void)
 * @brief     start globaltimer2 which is for customer using
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-10
 * @todo      N/A
 * @bug       N/A
*/
void HAL_GlobalTimer2Start(void)
{
	XBYTE[0x20C0] &= 0xF7; /*clear global timer2 interrupt event*/
	XBYTE[0x20cc] = 0x01;	
}
/**
 * @fn        void HAL_GlobalTimer2Stop(void)
 * @brief     Stop globaltimer2 which is for customer using
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-10
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_GlobalTimer2Stop(void)
{
	XBYTE[0x20C0] &= 0xF7; /*clear global timer2 interrupt event*/
	XBYTE[0x20cc] = 0x02;

	return 0;
}

/**
 * @fn        UINT8 HAL_GlobalTimer2IntEvt(void)
 * @brief     get global timer2 interrupt event
 * @param     NONE
 * @retval    0: the intrrupt is not generated
 *			  1: the intrrupt is generated
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_GlobalTimer2IntEvtGet(void)
{
	if (XBYTE[0x20C0] & 0x08) {
		XBYTE[0x20C0] &= ~0x08;//
		return 1;
	}
	else {
		return 0;
	}	
}


/**
 * @fn        UINT8 HAL_GlobalWDTCfg(UINT32 initval, UINT8 countmode, UINT8 enable)
 * @brief     set watch dog timer,time stick 1ms
 * @param     [in] initval: initial value
 * @param     [in] countmode
 *				0: down count
 *				1: up count
 * @param     [in] enable
 *				0: stop watch dog timer
 *				1: start watch dog timer
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-10
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_GlobalWDTCfg(UINT32 initval, UINT8 countmode, UINT8 enable)
{
	XBYTE[0x2075] = 0x02;	/*stop WDT*/

	if ( countmode == 0x01 ) { /*up count mode and time stick 1ms*/
		XBYTE[0x2074] |= 0x03;
	}	
	else if( countmode == 0x00 ) { /*down count mode and time stick 1ms*/
		XBYTE[0x2074] |= 0x02;
	}
	else {
		return FAIL;
	}
	XBYTE[0x2070] = READ8(initval,3);//(UINT8) (initval & 0xff);	/* set initial value */
	XBYTE[0x2071] = READ8(initval,2);//(UINT8) ((initval>>8) & 0xff);
	XBYTE[0x2072] = READ8(initval,1);//(UINT8) ((initval>>16) & 0xff);
	XBYTE[0x2073] = READ8(initval,0);//(UINT8) ((initval>>24) & 0xff);
	XBYTE[0x2075] |= 0x04;	/*load time*/	

	initval = 1;
	XBYTE[0x2076] = READ8(initval,3);//(UINT8) (initval & 0xff);	/* set initial value */
	XBYTE[0x2077] = READ8(initval,2);//(UINT8) ((initval>>8) & 0xff);
	XBYTE[0x2078] = READ8(initval,1);//(UINT8) ((initval>>16) & 0xff);
	XBYTE[0x2079] = READ8(initval,0);//(UINT8) ((initval>>24) & 0xff);

	XBYTE[0x20D0] |= 0x02;   // timer0 int enable
	XBYTE[0x2074] |= 0x04;	/*enable WDT REST*/
	
	if( enable == 0x01) {	/*start WDT*/
		XBYTE[0x2075] = 0x01;	
	}
	else if ( enable == 0x00 ) {	/*stop WDT*/
		XBYTE[0x2075] = 0x02;
	}
	else {
		return FAIL;
	}
	
	return SUCCESS;
}

/**
 * @fn        void HAL_GlobalWDTSet(UINT8 enable)
 * @brief     HAL_GlobalWDTSet
 * @param     [in] enable
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-7-5
 * @todo      N/A
 * @bug       N/A
*/
void HAL_GlobalWDTSet(UINT8 enable)
{
	if (enable) {
		XBYTE[0x2075] = 0x01;	/*start WDT*/
	} else {
		XBYTE[0x2075] = 0x02;	/*stop WDT*/
	}
}
 /**
 * @fn        void HAL_GlobalWDTTimeLoad(UINT32 loadtime)
 * @brief     load the WDT time
 * @param     [in] loadtime
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-7
 * @todo      N/A
 * @bug       N/A
*/
void HAL_GlobalWDTTimeLoad(UINT32 loadtime)
{
	XBYTE[0x2070] = READ8(loadtime,3);//(UINT8) (loadtime & 0xff);	/* set initial value */
	XBYTE[0x2071] = READ8(loadtime,2);//(UINT8) ((loadtime>>8) & 0xff);
	XBYTE[0x2072] = READ8(loadtime,1);//(UINT8) ((loadtime>>16) & 0xff);
	XBYTE[0x2073] = READ8(loadtime,0);//(UINT8) ((loadtime>>24) & 0xff);
	
	XBYTE[0x2075] |= 0x04;	/*load time*/	
}

 /**
 * @fn        void HAL_GlobalPwrMosSet(UINT8 en)
 * @brief     HAL_GlobalPwrMosSet
 * @param     [in] en: enable
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-7
 * @todo      N/A
 * @bug       N/A
*/
void HAL_GlobalPwrMosSet(UINT8 en)
{
	if (en) {
		XBYTE[0x2085] |= 0x01;
		HAL_GlobalTimerWait(2);
		XBYTE[0x2085] |= 0x02;
		HAL_GlobalTimerWait(2);
		XBYTE[0x2085] |= 0x04;
	} else {
		XBYTE[0x2085] &= ~0x07;
	}
}

/**
 * @fn        void HAL_GlobalUartSet(UINT8 en)
 * @brief     HAL_GlobalUartSet
 * @param     [in] en
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-15
 * @todo      N/A
 * @bug       N/A
*/
void HAL_GlobalUartSet(UINT8 en)
{
	if (en) {
		XBYTE[0x201A] |= 0x60;
	} else {
		XBYTE[0x201A] &= ~0x60;
	}
}

/**
 * @fn		  void HAL_GlobalRTCDrivingSet(UINT8 val)
 * @brief	  Set RTC Driving
 * @param	  [in] val :Operating current programminig
 *                  000:0.7uA 
 *                  001:1uA 
 *                  010:1.5uA 
 *                  011:2uA 
 *                  100:2.5uA 
 *                  101:3uA 
 *                  110:4uA 
 *                  111:10uA
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2011-5-4
 * @todo	  N/A
 * @bug 	  N/A
*/
void HAL_GlobalRTCDrivingSet(UINT8 val)
{
	if ( val < 8) {
		HAL_GlobalWriteRTCData(0x05, val); //Write RTC Driving
	}
}

/**
 * @fn		  void HAL_GlobalICVerGet(void)
 * @brief	  Get IC version
 * @param	  NONE
 * @retval	  IC version
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2011-5-4
 * @todo	  N/A
 * @bug 	  N/A
*/
#pragma disable
UINT8 HAL_GlobalICVerGet(void)
{
	#if IC_VER_DETECT
	return XBYTE[0x20FF]; 
	#else
	return 0x00; //disable auto detection of IC version  
	#endif
}

/**
 * @fn		  void HAL_GlobalICVerGet_intr(void)
 * @brief	  Get IC version for intr svc
 * @param	  NONE
 * @retval	  IC version
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2011-5-4
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 HAL_GlobalICVerGet_intr(void)
{
	#if IC_VER_DETECT
	return XBYTE[0x20FF]; 
	#else
	return 0x00; //disable auto detection of IC version  
	#endif
}

/**
 * @fn		  void HAL_GlobalDGPIOModeGet(void)
 * @brief	  Get IC version for APP
 * @param	  NONE
 * @retval	  IC version
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2011-5-4
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 HAL_GlobalDGPIOModeGet(void)
{
	return HAL_GlobalICVerGet();
}
extern UINT16 tvTime;
#if 1
void HAL_GlobalAlarmRTCIntrSet_Intr()
{
	UINT8 first0, first1,first2,first3,first4,first5;
  	UINT32 _hight;
 	UINT16 _low; 
	HAL_GlobalWriteRTCData(0x00, 0x05);	
	HAL_GlobalWriteRTCData(0xB1, 0x01);
	HAL_GlobalTimerWait10us(5);
	HAL_GlobalReadRTCData(0x15, &first5);
	HAL_GlobalReadRTCData(0x14, &first4);	
	HAL_GlobalReadRTCData(0x13, &first3);	
	HAL_GlobalReadRTCData(0x12, &first2);	
	HAL_GlobalReadRTCData(0x11, &first1);	
	HAL_GlobalReadRTCData(0x10, &first0);	
	WRITE8(_low,0,first1);
	WRITE8(_low,1,first0);
	WRITE8(_hight,0,first5);
	WRITE8(_hight,1,first4);
	WRITE8(_hight,2,first3);
	WRITE8(_hight,3,first2);

   if(_low >=(65536 - tvTime) ){
     _hight += 1;
   }
	_low += tvTime;
	first0 = READ8(_low,1);
	first1 = READ8(_low,0);

	first2 = READ8(_hight,3);
	first3 = READ8(_hight,2);
	first4 = READ8(_hight,1);
	first5= READ8(_hight,0);	
	HAL_GlobalWriteRTCData(0x20, first0);
	HAL_GlobalWriteRTCData(0x21, first1);
	HAL_GlobalWriteRTCData(0x22, first2);
	HAL_GlobalWriteRTCData(0x23, first3);
	HAL_GlobalWriteRTCData(0x24, first4);
	HAL_GlobalWriteRTCData(0x25, first5);
	Hal_GlobalAlarmRTCIntrSet(1);
	return ;
}
#endif
void HAL_GlobalRTCSecondIntrSet(UINT8 enable)
{
	//UINT8 value = 0;
	//HAL_GlobalReadRTCData(0xc0, &value); /*read flag*/
	HAL_GlobalWriteRTCData(0xc0, 0); /*clear alarm*/
	if(enable){
		HAL_GlobalWriteRTCData(0xd0, 0x01); /*enable alarm*/
		//XBYTE[0x20EB] |= 0x01;//Enable RTC interrupt
		XBYTE[0x20D0] |= 0x20;//Enable RTC interrupt
	}
	else{
		HAL_GlobalWriteRTCData(0xd0, 0); /*disable alarm*/
		//XBYTE[0x20EB] &= 0xfe;//Disable RTC interrupt
		XBYTE[0x20D0] &= (~0x20);//Disable RTC interrupt
	}
}
