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
 * @file      hal_front_common.c
 * @brief     The common file for front driver
 * @author    Will Chiu
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "front_logi.h"
#include "sensor_cdsp.h"
#include "ae_api.h"
#include "hal_front_common.h"
#include "hal.h"
#include "hal_global.h"
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
static UINT8 waitFlag=0; 
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
 extern UINT8 xdata Gt1Cnt;
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**
 * @fn        void HAL_FrontAeGet(UINT16 idx, UINT8 opt, UINT32 *shutter, UINT16 *gain)
 * @brief     Get exposure time and gain 
 * @param     [in] idx : AE table idx
 * @param     [in] opt : 
 *						60: 60hz AE table
 *						50: 50hz AE table 	
 * @param     [in] shutter : exposure time (time= 10/value)
 * @param     [in] gain : gain value of sensor
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_FrontAeGet(
	UINT16 idx,
	UINT8 opt,
	UINT32 *shutter,
	UINT16 *gain
)
{
	frontPrevCapabDesc_t *pcap;

	frontCapabilityGet(0, &pcap, NULL);
	if ( opt == 60 ) {
		*shutter = pcap->ae60hzTbl[ idx ].shutter;
		*gain = pcap->ae60hzTbl[ idx ].gain;
	}
	else {
		*shutter = pcap->ae50hzTbl[ idx ].shutter;
		*gain = pcap->ae50hzTbl[ idx ].gain;
	}

	return;
}

/**
 * @fn        void HAL_Front10UsWait(UINT32 time_wait)
 * @brief     wiat 
 * @param     [in] time_wait : time unit is 10 us
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_Front10UsWait(
	UINT16 time_wait
)
{

	//HAL_GlobalTimerWait10us((UINT8)time_wait);  /* wait for kai to revise function */


	
	#if 0
		//UINT32 cnt;
		
		//cnt = (UINT32) 0xFFFFFFFF - time_wait;

		XBYTE[0x20c7] = *((UINT8*)&time_wait + 3 );//read8(time_wait,3);//
		XBYTE[0x20c8] = *((UINT8*)&time_wait + 2 );//read8(time_wait,2);//
		XBYTE[0x20c9] = *((UINT8*)&time_wait + 1 );//read8(time_wait,1);//
		XBYTE[0x20ca] = *((UINT8*)&time_wait + 0 );//read8(time_wait,0);//
		
		HAL_GlobalTimer2Start();	/*start timer2*/ 

		while(!waitFlag);
		//while((XBYTE[0x20c7] != 0x00) || (XBYTE[0x20c8] != 0x00) || (XBYTE[0x20c9] != 0)){
		//	printf("0x20c7=%bx\n",XBYTE[0x20c7]);
		//}
		
	 	//while(!HAL_GlobalTimer2IntEvtGet());
	 	
	 	//HAL_GlobalTimer2Stop();	/*start timer2*/ 
	 	waitFlag=0;
	#else
		#if 1
		while(time_wait>200){
			HAL_GlobalTimerWait10us(200);
			time_wait-=200;					
		}

		if (time_wait>0){
			HAL_GlobalTimerWait10us((UINT8)(time_wait&0xff));		
		}

		
		#endif

		#if 0
		UINT8 sts;
		//UINT8 t1,t2,t3,t4;
		//UINT8 cnt;
		//t1=XBYTE[0x20c1];
		//t2=XBYTE[0x20c2];		
		ENTER_CRITICAL(sts);

		XBYTE[0x20c6] = 0x02;	/*stop timer1 */   
		XBYTE[0x20C5] = 0x00;
		XBYTE[0x20d0] |= 0x04;
		//time_wait = (UINT32) 0xFFFFFFFF - time_wait;

		XBYTE[0x20c1] = *((UINT8*)&time_wait + 3 );//read8(time_wait,3);//
		XBYTE[0x20c2] = *((UINT8*)&time_wait + 2 );//read8(time_wait,2);//
		XBYTE[0x20c3] = *((UINT8*)&time_wait + 1 );//read8(time_wait,1);//
		XBYTE[0x20c4] = 0;

		//t3=XBYTE[0x20c1];
		//t4=XBYTE[0x20c2];

		//printf("start=%bx,%bx\n",t1,t2);
		//printf("end=%bx,%bx\n",t3,t4);

		XBYTE[0x20c6] = 0x04;   /*load timer1 value*/

		XBYTE[0x20c6] = 0x01;	/*start timer1 */   
		//cnt=Gt1Cnt;
		//while((XBYTE[0x20c1] != 0x00) || (XBYTE[0x20c2] != 0x00) || (XBYTE[0x20c3] != 0x00));
		while(!(XBYTE[0x20c0]&0x04));

		EXIT_CRITICAL(sts);

		XBYTE[0x20C5] = 0x01;
		#endif
		#if 0
		UINT8 sts;

		ENTER_CRITICAL(sts);

		XBYTE[0x2074] = 0x02;

		time_wait = (UINT32) 0xFFFFFFFF - time_wait;

		XBYTE[0x2070] = M_LoByteOfDword(time_wait);
		XBYTE[0x2071] = M_MidLoByteOfDword(time_wait);
		XBYTE[0x2072] = M_MidHiByteOfDword(time_wait);

		XBYTE[0x2074] = 0x01;

		while((XBYTE[0x2070] != 0x00) || (XBYTE[0x2071] != 0x00) || (XBYTE[0x2072] != 0));

		EXIT_CRITICAL(sts);
		#endif
	#endif	
}

#if 0  // mattwang remove.
void
HalFrontAeSet(
	UINT16 idx,
	UINT8 opt
)
{
	UINT32 shutter;
	UINT16 gain;
	frontPrevCapabDesc_t *pcap;


	frontCapabilityGet(0, &pcap, NULL);
	if ( opt == 60 ) {
		shutter = pcap->ae60hzTbl[ idx ].shutter;
		gain = pcap->ae60hzTbl[ idx ].gain;
	}
	else {
		shutter = pcap->ae50hzTbl[ idx ].shutter;
		gain = pcap->ae50hzTbl[ idx ].gain;
	}

	G_AEReady = 1;
	G_SHUTTER = shutter;
	G_GAIN = gain;

	XBYTE[REG_Cdsp_Cdspinten_lo] = XBYTE[REG_Cdsp_Cdspinten_lo] | 0x04;//AE,AWB interrupt & CDSP Eof
	HAL_FrontIntrEnable(1, 0x41);

	while( !( G_AEReady == 0 && G_AESetting == 0 ) );

	XBYTE[REG_Cdsp_Cdspinten_lo] = XBYTE[REG_Cdsp_Cdspinten_lo] & 0xFB;//AE window done intr disable

	return;
}
#endif

